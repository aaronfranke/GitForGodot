tool
class_name LibGit2Defines
# This file implements some of the constants and enums defined in LibGit2.

# Bitwise enum.
enum GitBranch {
	LOCAL = 1,
	REMOTE = 2,
	ANY = 3, # Union of 1 and 2.
	HEAD = 4,
}

# Contiguous enum.
enum GitDelta {
	UNMODIFIED = 0,  # No changes.
	ADDED = 1,       # Entry does not exist in old version.
	DELETED = 2,     # Entry does not exist in new version.
	MODIFIED = 3,    # Entry content changed between old and new.
	RENAMED = 4,     # Entry was renamed between old and new.
	COPIED = 5,      # Entry was copied from another old entry.
	IGNORED = 6,     # Entry is ignored item in workdir.
	UNTRACKED = 7,   # Entry is untracked item in workdir.
	TYPECHANGE = 8,  # Type of entry changed between old and new.
	UNREADABLE = 9,  # Entry is unreadable.
	CONFLICTED = 10, # Entry in the index is conflicted.
}

# Bitwise enum.
enum GitStatus {
	CURRENT = 0,

	INDEX_NEW        = (1 << 0),
	INDEX_MODIFIED   = (1 << 1),
	INDEX_DELETED    = (1 << 2),
	INDEX_RENAMED    = (1 << 3),
	INDEX_TYPECHANGE = (1 << 4),

	WT_NEW           = (1 << 7),
	WT_MODIFIED      = (1 << 8),
	WT_DELETED       = (1 << 9),
	WT_TYPECHANGE    = (1 << 10),
	WT_RENAMED       = (1 << 11),
	WT_UNREADABLE    = (1 << 12),

	IGNORED          = (1 << 14),
	CONFLICTED       = (1 << 15),
}

const GIT_STATUS_ANY_NEW = GitStatus.INDEX_NEW | GitStatus.WT_NEW
const GIT_STATUS_ANY_MODIFIED = GitStatus.INDEX_MODIFIED | GitStatus.WT_MODIFIED
const GIT_STATUS_ANY_DELETED = GitStatus.INDEX_DELETED | GitStatus.WT_DELETED
const GIT_STATUS_ANY_RENAMED = GitStatus.INDEX_RENAMED | GitStatus.WT_RENAMED
const GIT_STATUS_ANY_TYPECHANGE = GitStatus.INDEX_TYPECHANGE | GitStatus.WT_TYPECHANGE

const GIT_STATUS_INDEX_CHANGES = GitStatus.INDEX_NEW | GitStatus.INDEX_MODIFIED | GitStatus.INDEX_DELETED | GitStatus.INDEX_RENAMED | GitStatus.INDEX_TYPECHANGE
const GIT_STATUS_WT_CHANGES = GitStatus.WT_NEW | GitStatus.WT_MODIFIED | GitStatus.WT_DELETED | GitStatus.WT_RENAMED | GitStatus.WT_TYPECHANGE | GitStatus.WT_UNREADABLE
