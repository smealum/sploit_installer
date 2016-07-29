The actual exploit builds are contained under the "romfs" directory which you must create yourself, when building manually. See also the "romfs_example" directory. Hence, the application itself doesn't contain anything that's specific to any particular exploit, everything exploit-specific is stored in romfs.

# romfs/exploitlist_config
Each non-empty line in this config file is for a different exploit, in the following format: "{exploitname} {titlename} {flags} {list of programIDs with arbitrary number of programIDs}"  
Flags bitmasks:
* 0x1: Enable compressing the payload.
* 0x2: Enable using the input save directories "Old3DS"/"New3DS", depending on the selected system model.
* 0x4: Enable using the input save directory "common".

# romfs/{exploitname}/{programID}/
The "config.ini" file in this directory contains the list of versions for this title, this is used by sploit_installer for automatically detecting which version to use.  

"[updatetitle_versions]" section(required when an update-title is detected):
* v{titleversion}={remaster_version}

"[remaster_versions]" section(required):
* {remaster_version}={absolute directory-path for this version}@{display version}

# version-directory
This contains the "Old3DS"/"New3DS" and/or "common" directories mentioned in the above "romfs/exploitlist_config" section. Those directories contain config.ini:
* {inputrelative_savefilepath}={outputabsolute_savefilepath}

This lists the file(s) which get copied into the title's savedata. The input/output filepaths in this config can include "@!dX". That "@!dX" string will be replaced with the output of: snprintf(..., "%0Xd", selected_save_slot), where selected_save_slot is 0-based. Hence, "@!d1" == "%01d", "@!d2" == "%02d", and so on.
A file containing "@!pX" will have the otherapp payload embedded within it, at the offset "X", with a u32 payload size preceding the actual payload.
