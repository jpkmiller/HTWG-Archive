# inspired by page 165 and https://codywu2010.wordpress.com/2015/09/27/cpuset-by-example/

/bin/echo "Setup for rt arch..."
mkdir /dev/cpuset
mount -t cpuset cpuset /dev/cpuset

/bin/echo "Create cgroups..."
# setup cgroups
mkdir /dev/cpuset/non-rt
mkdir /dev/cpuset/rt


/bin/echo "Config cgroups..."
# see here for more information about configurations https://linux.die.net/man/7/cpuset

# non-rt setup
/bin/echo 0 >/dev/cpuset/non-rt/cpuset.mems
/bin/echo 0-1 >/dev/cpuset/non-rt/cpuset.cpus
/bin/echo 1 >/dev/cpuset/non-rt/cpuset.memory_migrate
/bin/echo 0 >/dev/cpuset/non-rt/cpuset.sched_load_balance

# rt setup
/bin/echo 2 >/dev/cpuset/rt/cpuset.cpus
/bin/echo 0 >/dev/cpuset/rt/cpuset.mems
/bin/echo 1 >/dev/cpuset/rt/cpuset.cpu_exclusive

/bin/echo 1 > /dev/cpuset/cpuset.memory_pressure_enabled

/bin/echo "Setup non-rt tasks"
# set all tasks to non-rt
while read -r pid; do
  /bin/echo "$pid"
  /bin/echo "$pid" >/dev/cpuset/non-rt/tasks
done </dev/cpuset/tasks

/bin/echo "Setup rt task $1"
# $1 should be pid and should be executed as rt task
/bin/echo "$1" >/dev/cpuset/rt/tasks

if [ "$(cat /dev/cpuset/rt/tasks)" -eq "$1" ]; then
  /bin/echo "Successfully set $1 as rt task"
fi

/bin/echo "Finished!"
