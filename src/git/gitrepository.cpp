#include "gitrepository.h"
#include "gitclient.h"
#include <git2.h>
#include <QDebug>
#include <QFileInfo>
#include <stdexcept>

GitRepository::GitRepository(const QString &path)
    : m_repo(nullptr)
    , m_path(path)
{
    GitClient::initialize();
    
    int error = git_repository_open(&m_repo, path.toUtf8().constData());
    if (error != 0) {
        throw std::runtime_error(GitClient::getLastError().toStdString());
    }
}

GitRepository::~GitRepository()
{
    if (m_repo) {
        git_repository_free(m_repo);
    }
}

void GitRepository::clone(const QString &url, const QString &path)
{
    QString error;
    if (!GitClient::clone(url, path, error)) {
        throw std::runtime_error(error.toStdString());
    }
}

void GitRepository::init(const QString &path)
{
    QString error;
    if (!GitClient::init(path, error)) {
        throw std::runtime_error(error.toStdString());
    }
}

QString GitRepository::currentBranch() const
{
    if (!m_repo) return QString();
    
    git_reference *head = nullptr;
    int error = git_repository_head(&head, m_repo);
    
    if (error != 0) {
        return QString("(detached)");
    }
    
    const char *branch_name = git_reference_shorthand(head);
    QString result = QString::fromUtf8(branch_name);
    
    git_reference_free(head);
    return result;
}

bool GitRepository::hasUncommittedChanges() const
{
    return !status().empty();
}

QStringList GitRepository::branches(bool includeRemote) const
{
    QStringList result;
    
    git_branch_iterator *iter = nullptr;
    git_reference *ref = nullptr;
    git_branch_t type;
    
    int error = git_branch_iterator_new(&iter, m_repo, GIT_BRANCH_LOCAL);
    if (error != 0) return result;
    
    while (git_branch_next(&ref, &type, iter) == 0) {
        const char *branch_name = nullptr;
        git_branch_name(&branch_name, ref);
        result << QString::fromUtf8(branch_name);
        git_reference_free(ref);
    }
    
    git_branch_iterator_free(iter);
    
    if (includeRemote) {
        error = git_branch_iterator_new(&iter, m_repo, GIT_BRANCH_REMOTE);
        if (error == 0) {
            while (git_branch_next(&ref, &type, iter) == 0) {
                const char *branch_name = nullptr;
                git_branch_name(&branch_name, ref);
                result << QString::fromUtf8(branch_name);
                git_reference_free(ref);
            }
            git_branch_iterator_free(iter);
        }
    }
    
    return result;
}

std::vector<GitBranch> GitRepository::getBranches() const
{
    std::vector<GitBranch> result;
    
    git_branch_iterator *iter = nullptr;
    git_reference *ref = nullptr;
    git_branch_t type;
    
    QString current = currentBranch();
    
    int error = git_branch_iterator_new(&iter, m_repo, GIT_BRANCH_ALL);
    if (error != 0) return result;
    
    while (git_branch_next(&ref, &type, iter) == 0) {
        const char *branch_name = nullptr;
        git_branch_name(&branch_name, ref);
        
        GitBranch branch;
        branch.name = QString::fromUtf8(branch_name);
        branch.isHead = (branch.name == current);
        branch.isRemote = (type == GIT_BRANCH_REMOTE);
        
        result.push_back(branch);
        git_reference_free(ref);
    }
    
    git_branch_iterator_free(iter);
    return result;
}

void GitRepository::createBranch(const QString &name, const QString &startPoint)
{
    git_reference *ref = nullptr;
    git_commit *commit = nullptr;
    
    // Get HEAD commit or specified start point
    git_object *target = nullptr;
    if (startPoint.isEmpty()) {
        int error = git_revparse_single(&target, m_repo, "HEAD");
        throwIfError(error, "get HEAD");
    } else {
        int error = git_revparse_single(&target, m_repo, startPoint.toUtf8().constData());
        throwIfError(error, "get start point");
    }
    
    git_commit_lookup(&commit, m_repo, git_object_id(target));
    
    int error = git_branch_create(&ref, m_repo, name.toUtf8().constData(), commit, 0);
    throwIfError(error, "create branch");
    
    git_reference_free(ref);
    git_commit_free(commit);
    git_object_free(target);
}

void GitRepository::checkoutBranch(const QString &name)
{
    git_reference *ref = nullptr;
    git_object *treeish = nullptr;
    
    QString refName = "refs/heads/" + name;
    int error = git_reference_lookup(&ref, m_repo, refName.toUtf8().constData());
    throwIfError(error, "lookup branch");
    
    error = git_reference_peel(&treeish, ref, GIT_OBJECT_TREE);
    throwIfError(error, "peel reference");
    
    git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
    opts.checkout_strategy = GIT_CHECKOUT_SAFE;
    
    error = git_checkout_tree(m_repo, treeish, &opts);
    throwIfError(error, "checkout tree");
    
    error = git_repository_set_head(m_repo, refName.toUtf8().constData());
    throwIfError(error, "set HEAD");
    
    git_object_free(treeish);
    git_reference_free(ref);
}

void GitRepository::deleteBranch(const QString &name)
{
    git_reference *ref = nullptr;
    
    QString refName = "refs/heads/" + name;
    int error = git_reference_lookup(&ref, m_repo, refName.toUtf8().constData());
    throwIfError(error, "lookup branch");
    
    error = git_branch_delete(ref);
    throwIfError(error, "delete branch");
}

std::vector<GitCommit> GitRepository::log(int maxCount) const
{
    std::vector<GitCommit> result;
    
    git_revwalk *walker = nullptr;
    int error = git_revwalk_new(&walker, m_repo);
    if (error != 0) return result;
    
    git_revwalk_sorting(walker, GIT_SORT_TIME);
    git_revwalk_push_head(walker);
    
    git_oid oid;
    int count = 0;
    
    while (git_revwalk_next(&oid, walker) == 0 && count < maxCount) {
        git_commit *commit = nullptr;
        if (git_commit_lookup(&commit, m_repo, &oid) == 0) {
            GitCommit gc;
            
            char oid_str[GIT_OID_HEXSZ + 1];
            git_oid_tostr(oid_str, sizeof(oid_str), &oid);
            gc.oid = QString::fromUtf8(oid_str);
            
            gc.message = QString::fromUtf8(git_commit_message(commit));
            
            const git_signature *author = git_commit_author(commit);
            gc.author = QString::fromUtf8(author->name);
            gc.email = QString::fromUtf8(author->email);
            gc.timestamp = QDateTime::fromSecsSinceEpoch(author->when.time);
            
            // Get parent commits
            unsigned int parent_count = git_commit_parentcount(commit);
            for (unsigned int i = 0; i < parent_count; i++) {
                const git_oid *parent_oid = git_commit_parent_id(commit, i);
                char parent_str[GIT_OID_HEXSZ + 1];
                git_oid_tostr(parent_str, sizeof(parent_str), parent_oid);
                gc.parents << QString::fromUtf8(parent_str);
            }
            
            result.push_back(gc);
            git_commit_free(commit);
            count++;
        }
    }
    
    git_revwalk_free(walker);
    return result;
}

void GitRepository::commit(const QString &message, const QStringList &files)
{
    // If specific files are provided, stage them
    if (!files.isEmpty()) {
        for (const QString &file : files) {
            stageFile(file);
        }
    }
    
    git_index *index = nullptr;
    int error = git_repository_index(&index, m_repo);
    throwIfError(error, "get index");
    
    git_oid tree_oid, commit_oid;
    git_tree *tree = nullptr;
    
    error = git_index_write_tree(&tree_oid, index);
    throwIfError(error, "write tree");
    
    error = git_tree_lookup(&tree, m_repo, &tree_oid);
    throwIfError(error, "lookup tree");
    
    git_signature *sig = getSignature();
    
    git_reference *head = nullptr;
    error = git_repository_head(&head, m_repo);
    
    git_commit *parent = nullptr;
    const git_commit *parents[1];
    int parent_count = 0;
    
    if (error == 0) {
        git_oid parent_oid;
        git_reference_name_to_id(&parent_oid, m_repo, "HEAD");
        git_commit_lookup(&parent, m_repo, &parent_oid);
        parents[0] = parent;
        parent_count = 1;
    }
    
    error = git_commit_create_v(
        &commit_oid,
        m_repo,
        "HEAD",
        sig,
        sig,
        nullptr,
        message.toUtf8().constData(),
        tree,
        parent_count,
        parent_count > 0 ? parents[0] : nullptr
    );
    
    throwIfError(error, "create commit");
    
    if (parent) git_commit_free(parent);
    if (head) git_reference_free(head);
    git_tree_free(tree);
    git_signature_free(sig);
    git_index_free(index);
}

void GitRepository::stageFile(const QString &path)
{
    git_index *index = nullptr;
    int error = git_repository_index(&index, m_repo);
    throwIfError(error, "get index");
    
    error = git_index_add_bypath(index, path.toUtf8().constData());
    throwIfError(error, "stage file");
    
    error = git_index_write(index);
    throwIfError(error, "write index");
    
    git_index_free(index);
}

void GitRepository::unstageFile(const QString &path)
{
    git_index *index = nullptr;
    int error = git_repository_index(&index, m_repo);
    throwIfError(error, "get index");
    
    error = git_index_remove_bypath(index, path.toUtf8().constData());
    throwIfError(error, "unstage file");
    
    error = git_index_write(index);
    throwIfError(error, "write index");
    
    git_index_free(index);
}

void GitRepository::stageAll()
{
    git_index *index = nullptr;
    int error = git_repository_index(&index, m_repo);
    throwIfError(error, "get index");
    
    error = git_index_add_all(index, nullptr, 0, nullptr, nullptr);
    throwIfError(error, "stage all");
    
    error = git_index_write(index);
    throwIfError(error, "write index");
    
    git_index_free(index);
}

std::vector<GitFileStatus> GitRepository::status() const
{
    std::vector<GitFileStatus> result;
    
    git_status_list *status_list = nullptr;
    git_status_options opts = GIT_STATUS_OPTIONS_INIT;
    opts.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
    opts.flags = GIT_STATUS_OPT_INCLUDE_UNTRACKED |
                 GIT_STATUS_OPT_RENAMES_HEAD_TO_INDEX |
                 GIT_STATUS_OPT_SORT_CASE_SENSITIVELY;
    
    int error = git_status_list_new(&status_list, m_repo, &opts);
    if (error != 0) return result;
    
    size_t count = git_status_list_entrycount(status_list);
    
    for (size_t i = 0; i < count; i++) {
        const git_status_entry *entry = git_status_byindex(status_list, i);
        
        GitFileStatus fs;
        
        if (entry->head_to_index) {
            fs.path = QString::fromUtf8(entry->head_to_index->new_file.path);
        } else if (entry->index_to_workdir) {
            fs.path = QString::fromUtf8(entry->index_to_workdir->new_file.path);
        }
        
        unsigned int status = entry->status;
        
        if (status & GIT_STATUS_INDEX_NEW || status & GIT_STATUS_WT_NEW) {
            fs.status = GitFileStatus::Added;
        } else if (status & GIT_STATUS_INDEX_MODIFIED || status & GIT_STATUS_WT_MODIFIED) {
            fs.status = GitFileStatus::Modified;
        } else if (status & GIT_STATUS_INDEX_DELETED || status & GIT_STATUS_WT_DELETED) {
            fs.status = GitFileStatus::Deleted;
        } else if (status & GIT_STATUS_INDEX_RENAMED || status & GIT_STATUS_WT_RENAMED) {
            fs.status = GitFileStatus::Renamed;
        } else if (status & GIT_STATUS_CONFLICTED) {
            fs.status = GitFileStatus::Conflicted;
        } else {
            fs.status = GitFileStatus::Untracked;
        }
        
        result.push_back(fs);
    }
    
    git_status_list_free(status_list);
    return result;
}

QString GitRepository::diff(const QString &path) const
{
    git_diff *diff = nullptr;
    git_diff_options opts = GIT_DIFF_OPTIONS_INIT;
    opts.flags = GIT_DIFF_INCLUDE_UNTRACKED | GIT_DIFF_SHOW_UNTRACKED_CONTENT;
    
    // If a specific path is provided, filter to that path
    if (!path.isEmpty()) {
        git_strarray paths;
        const char *pathStr = path.toUtf8().constData();
        paths.strings = const_cast<char**>(&pathStr);
        paths.count = 1;
        opts.pathspec = paths;
    }
    
    // Get the index
    git_index *index = nullptr;
    int error = git_repository_index(&index, m_repo);
    if (error != 0) {
        return QString("Error getting index: %1").arg(GitClient::getLastError());
    }
    
    // Get diff between index and working directory
    error = git_diff_index_to_workdir(&diff, m_repo, index, &opts);
    git_index_free(index);
    
    if (error != 0) {
        return QString("Error generating diff: %1").arg(GitClient::getLastError());
    }
    
    // Convert diff to string
    QString result;
    
    git_diff_foreach(diff, 
        // file callback
        [](const git_diff_delta *delta, float progress, void *payload) -> int {
            Q_UNUSED(progress);
            QString *result = static_cast<QString*>(payload);
            
            const char *oldPath = delta->old_file.path;
            const char *newPath = delta->new_file.path;
            
            *result += QString("diff --git a/%1 b/%2\n").arg(oldPath, newPath);
            
            switch (delta->status) {
                case GIT_DELTA_ADDED:
                    *result += QString("new file\n");
                    break;
                case GIT_DELTA_DELETED:
                    *result += QString("deleted file\n");
                    break;
                case GIT_DELTA_MODIFIED:
                    *result += QString("modified\n");
                    break;
                case GIT_DELTA_RENAMED:
                    *result += QString("renamed from %1\n").arg(oldPath);
                    break;
                default:
                    break;
            }
            
            return 0;
        },
        nullptr, // binary callback
        // hunk callback
        [](const git_diff_delta *delta, const git_diff_hunk *hunk, void *payload) -> int {
            Q_UNUSED(delta);
            QString *result = static_cast<QString*>(payload);
            *result += QString::fromUtf8(hunk->header, hunk->header_len);
            return 0;
        },
        // line callback
        [](const git_diff_delta *delta, const git_diff_hunk *hunk, 
           const git_diff_line *line, void *payload) -> int {
            Q_UNUSED(delta);
            Q_UNUSED(hunk);
            QString *result = static_cast<QString*>(payload);
            
            char origin = line->origin;
            if (origin == GIT_DIFF_LINE_CONTEXT || 
                origin == GIT_DIFF_LINE_ADDITION || 
                origin == GIT_DIFF_LINE_DELETION) {
                *result += origin;
            }
            
            *result += QString::fromUtf8(line->content, line->content_len);
            return 0;
        },
        &result
    );
    
    git_diff_free(diff);
    
    if (result.isEmpty()) {
        return QString("No changes");
    }
    
    return result;
}

void GitRepository::push(const QString &remote, const QString &branch)
{
    git_remote *rem = nullptr;
    int error = git_remote_lookup(&rem, m_repo, remote.toUtf8().constData());
    throwIfError(error, "lookup remote");
    
    QString branchName = branch.isEmpty() ? currentBranch() : branch;
    QString refspec = QString("refs/heads/%1:refs/heads/%1").arg(branchName);
    
    const char *refspecs[] = { refspec.toUtf8().constData() };
    git_strarray refspec_array = { const_cast<char**>(refspecs), 1 };
    
    git_push_options opts = GIT_PUSH_OPTIONS_INIT;
    
    error = git_remote_push(rem, &refspec_array, &opts);
    throwIfError(error, "push");
    
    git_remote_free(rem);
}

void GitRepository::pull(const QString &remote, const QString &branch)
{
    // Fetch first
    fetch(remote);
    
    // Then merge
    // TODO: Implement merge logic
}

void GitRepository::fetch(const QString &remote)
{
    git_remote *rem = nullptr;
    int error = git_remote_lookup(&rem, m_repo, remote.toUtf8().constData());
    throwIfError(error, "lookup remote");
    
    git_fetch_options opts = GIT_FETCH_OPTIONS_INIT;
    
    error = git_remote_fetch(rem, nullptr, &opts, nullptr);
    throwIfError(error, "fetch");
    
    git_remote_free(rem);
}

QStringList GitRepository::remotes() const
{
    QStringList result;
    
    git_strarray remotes_array;
    int error = git_remote_list(&remotes_array, m_repo);
    
    if (error == 0) {
        for (size_t i = 0; i < remotes_array.count; i++) {
            result << QString::fromUtf8(remotes_array.strings[i]);
        }
        git_strarray_dispose(&remotes_array);
    }
    
    return result;
}

void GitRepository::addRemote(const QString &name, const QString &url)
{
    git_remote *remote = nullptr;
    int error = git_remote_create(&remote, m_repo, 
                                  name.toUtf8().constData(), 
                                  url.toUtf8().constData());
    throwIfError(error, "add remote");
    
    git_remote_free(remote);
}

QString GitRepository::getConfig(const QString &key) const
{
    git_config *cfg = nullptr;
    int error = git_repository_config(&cfg, m_repo);
    if (error != 0) return QString();
    
    const char *value = nullptr;
    error = git_config_get_string(&value, cfg, key.toUtf8().constData());
    
    QString result;
    if (error == 0 && value) {
        result = QString::fromUtf8(value);
    }
    
    git_config_free(cfg);
    return result;
}

void GitRepository::setConfig(const QString &key, const QString &value)
{
    git_config *cfg = nullptr;
    int error = git_repository_config(&cfg, m_repo);
    throwIfError(error, "get config");
    
    error = git_config_set_string(cfg, key.toUtf8().constData(), 
                                  value.toUtf8().constData());
    throwIfError(error, "set config");
    
    git_config_free(cfg);
}

void GitRepository::configureUser(const QString &name, const QString &email)
{
    setConfig("user.name", name);
    setConfig("user.email", email);
}

git_signature* GitRepository::getSignature() const
{
    git_signature *sig = nullptr;
    
    // Try to get from config
    QString name = getConfig("user.name");
    QString email = getConfig("user.email");
    
    if (name.isEmpty()) name = "Unknown User";
    if (email.isEmpty()) email = "unknown@example.com";
    
    git_signature_now(&sig, name.toUtf8().constData(), email.toUtf8().constData());
    
    return sig;
}

void GitRepository::throwIfError(int error, const QString &operation) const
{
    if (error != 0) {
        QString msg = QString("Failed to %1: %2").arg(operation, GitClient::getLastError());
        throw std::runtime_error(msg.toStdString());
    }
}
