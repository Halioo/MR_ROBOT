# Absolute path to this script
DIRPATH=$(dirname "$0")
# Close simulator upon exit
trap "kill 0" EXIT

usage() {
  cat << EOF >&2
Usage: $PROGNAME [-v <version>] [-sim]

-v <version> : choose between 3 versions : src, hello and wip
               default version : wip
 -s <status> : launch the simulator

EOF
  exit 1
}


version="wip"

while [ -n "$1" ]; do # while loop starts
    case "$1" in
    -sim) sim=1;;
    -c) comp=1;;
    -v)
        version="$2"
        shift
        ;;
    *) echo "Option $1 not recognized" ;;
    esac
    shift
done


# Ensure simulator launches
if [ "$sim" ]
then
  echo "-----STARTING SIMULATOR-----"
  java -jar "$DIRPATH"/intox-java_1.8-v0.3.6.jar &
  echo ">Waiting for simulator to launch..."
  sleep 1
fi


# Run the compiled program
# If no arguments -> run wip version
# If any argument -> run src version

case $version in
  "src" )
    echo "-----STARTING SOURCE ROBOT-----"
    "$DIRPATH"/robot_pc_source ;;
  "hello" )
    echo "-----STARTING HELLO ROBOT-----"
    "$DIRPATH"/hello_robot-v0.8/bin/robot_pc ;;
  "wip" )
    echo ">Compiling WIP MrRobot..."
    make -s -C "$DIRPATH"/version_a_completer
    echo "-----STARTING WIP ROBOT-----"
    "$DIRPATH"/version_a_completer/bin/robot_pc ;;
  * )
    echo "__/!\__ VERSION INTROUVABLE __/!\__" ;;
esac
