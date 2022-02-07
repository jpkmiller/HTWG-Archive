while read -r pid; do
  echo "$pid"
  echo "$pid" >/dev/cpuset/tasks
done </dev/cpuset/non-rt/tasks

while read -r pid; do
  echo "$pid"
  echo "$pid" >/dev/cpuset/tasks
done </dev/cpuset/rt/tasks


rmdir /dev/cpuset/non-rt
rmdir /dev/cpuset/rt

umount /dev/cpuset