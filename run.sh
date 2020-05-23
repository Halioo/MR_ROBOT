
# TUTO pour utiliser ce fichier :
# lancer ce fichier via le terminal permet d'automatiquement
# compiler et lancer le code du robot

# il est conçu pour avoir plusieurs options de lancement
# -sim : permet de lancer le simulateur en même temps
# -debug : permet d'afficher les infos de debug
# (pas de rapport avec la macro -NDEBUG)

# par défaut la commande la plus simple à taper pour est :
# ./run.sh -sim

# autre option secondaire :
# -v <id de la version> : cette commande permettait de choisir
# de lancer soit <src>, le code de robot V1 fourni par Delatour,
# soit <hello> pour lancer helloRobot, soit <wip> (pour work in progress)
# qui permet de lancer la version de l'utilisateur
# cette option n'est plus utile depuis le passage à la V2, et la balise
# -v est mise par défaut à <wip>


# Absolute path to this script
DIRPATH=$(dirname "$0")
# Close simulator upon exit
trap "kill 0" EXIT

usage() {
  cat << EOF >&2
Usage: $PROGNAME [-v <version>] [-sim]

-v <version> : choose between 3 versions : src, hello and wip
               default version : wip
-sim <status> : launch the simulator

EOF
  exit 1
}


version="wip"

while [ -n "$1" ]; do # while loop starts
    case "$1" in
    -sim) sim=1;;
    -debug) debug=1;;
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
    if [ "$debug" ]
    then
      make -C "$DIRPATH"/version_a_completer
    else
      make -s -C "$DIRPATH"/version_a_completer
    fi
    echo "-----STARTING WIP SERVER-----"
    xterm -fa 'Monospace' -fs 15 -e "$DIRPATH"/version_a_completer/bin/robot_pc1 2>server.log &
    sleep 0.2
    echo "-----STARTING WIP CLIENT-----"
    xterm -fa 'Monospace' -fs 15 -e "$DIRPATH"/version_a_completer/bin/robot_pc2 2>client.log
    ;;
  * )
    echo "__/!\__ VERSION INTROUVABLE __/!\__" ;;
esac

# Reset properly the terminal if it hasn't been done
stty echo cooked
sleep 0.1
