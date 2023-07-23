# File Hauler
Simple command-line file transfer program written in C++. What sets this program apart is its ability to maintain the original folder structure while providing the option to filter out unwanted files based on their extension.

This program is extensively utilized in our scanning process at Friendly Shade, and I believe it could be beneficial to others as well.

## Usage
```console
   -e,  --verbose

   -l,  --loop
     Scan and transfer files repeatedly in an infinite loop

   -o=<skip|overwrite-if-newer|overwrite>,  --overwrite-action=<skip
      |overwrite-if-newer|overwrite>
     (required)  Overwrite action (what to do when a file with the same
     name already exists in destination)

   -t=<move|copy>,  --transfer-mode=<move|copy>
     (required)  Transfer mode

   -f=<string>,  --file-extensions=<string>  (accepted multiple times)
     list of file extensions to copy. Example: "tif, jpg, png" (leave empty
     to copy all files)

   -d=<string>,  --dst=<string>
     (required)  Path to destination directory

   -s=<string>,  --src=<string>
     (required)  Path to source directory

   --,  --ignore_rest
     Ignores the rest of the labeled arguments following this flag.

   -v,  --version
     Displays version information and exits.

   -h,  --help
     Displays usage information and exits.
```

## Examples
### Simple copy
Copy all files in the source directory to the destination directory. If a file with the same name already exists in the destination, it will be skipped. The folder structure will also be copied.
```bat
FileHauler --src="F:/src" --dst="F:/dst" --transfer-mode="copy" --overwrite-action="skip"
```
### Filtering
Move files with the extensions "jpg" and "tif" only. If a file with the same name already exists in the destination, it will be replaced by the new one, but only if the new file is newer. The folder structure will also be copied.
```bat
FileHauler --src="F:/src" --dst="F:/dst" --transfer-mode="move" --file-extensions="jpg, tif" --overwrite-action="overwrite-if-newer"
```
### Simple copy (using short parameter names)
```bat
FileHauler -s="F:/src" -d="F:/dst" -t="copy" -o="skip"
```
