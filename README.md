# File Hauler
Simple command-line file transfer program written in C++

## Usage
```console
   -e,  --verbose

   -l,  --loop
     Scan and transfer files repeatedly in an infinite loop

   -o=<skip|overwrite-if-newer|always-overwrite>,  --overwrite-action=<skip
      |overwrite-if-newer|always-overwrite>
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
