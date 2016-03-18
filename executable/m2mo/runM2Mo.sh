if [ $# -lt 6 ]
then
	echo "1 - nodeID "
	echo "2 - topology "
  echo "3 - hours "
  echo "4 - minutes"
  echo "5 - ip"
  echo "6 - port"
else
  ./m2mo $1 $5 $6 >> outEvol2_topology1.log &
  echo "MainExp($1,$2,$3,$4,00)"| matlab -glnx86 2>&1 | tee -a outHandShake2_topology1.log
fi
killall m2mo
