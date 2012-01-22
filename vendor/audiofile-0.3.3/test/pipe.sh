# pipe.sh
# This script provides a simple test of Audio File Library
# operation on non-seekable file handles.

(./pipe out | ./pipe) && echo "passed pipe test"
