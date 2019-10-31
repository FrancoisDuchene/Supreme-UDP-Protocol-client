#!/bin/bash
# Tests de base

# TEST 1 - RÉSEAU PARFAIT
echo "Test 1 - Envoi d'une image de 97474 bytes sur un réseau parfait"
./link_sim -p 8888 -P 9999 -s 4269 &
linksim_pid=$!
./receiver -o test1 ::1 9999 &
receiver_pid=$!
./../sender ::1 8888 -f rogue\ one.jpg &
sender_pid=$!

echo "Le sender a terminé son exécution"
wait $sender_pid

if [ $? -eq 0 ]
then
    echo " "
    echo "Test 1 - réussi"
else
    echo " "
    echo "Echec Test 1"
fi

kill -9 $linksim_pid $receiver_pid

# TEST 2 - DÉLAI LÉGER
echo "Test 2 - Envoi d'une image de 97474 bytes sur un réseau avec un léger délai (50 ms) et un jitter de 10"
./link_sim -p 8888 -P 9999 -d 50 -s 4269 &
linksim_pid=$!
./receiver -o test2 ::1 9999 &
receiver_pid=$!
./../sender ::1 8888 -f rogue\ one.jpg &
sender_pid=$!

echo "Le sender a terminé son exécution"
wait $sender_pid

if [ $? -eq 0 ]
then
    echo " "
    echo "Test 2 - réussi"
else
    echo " "
    echo "Echec Test 2"
fi

kill -9 $linksim_pid $receiver_pid

# TEST 3 - GRAND DÉLAI
echo "Test 3 - Envoi d'une image de 97474 bytes sur un réseau avec un fort délai (1000 ms) et un jitter de 200"
./link_sim -p 8888 -P 9999 -d 1000 -j 200 -s 4269 &
linksim_pid=$!
./receiver -o test3 ::1 9999 &
receiver_pid=$!
./../sender ::1 8888 -f rogue\ one.jpg &
sender_pid=$!

echo "Le sender a terminé son exécution"
wait $sender_pid

if [ $? -eq 0 ]
then
    echo " "
    echo "Test 3 - réussi"
else
    echo " "
    echo "Echec Test 3"
fi

kill -9 $linksim_pid $receiver_pid

# TEST 4 - FAIBLE TAUX DE PERTE
echo "Test 4 - Envoi d'une image de 97474 bytes sur un réseau avec un faible taux de perte (5/100)"
./link_sim -p 8888 -P 9999 -l 5 -s 4269 &
linksim_pid=$!
./receiver -o test4 ::1 9999 &
receiver_pid=$!
./../sender ::1 8888 -f rogue\ one.jpg &
sender_pid=$!

echo "Le sender a terminé son exécution"
wait $sender_pid

if [ $? -eq 0 ]
then
    echo " "
    echo "Test 4 - réussi"
else
    echo " "
    echo "Echec Test 4"
fi

kill -9 $linksim_pid $receiver_pid

# TEST 5 - FAIBLE TAUX DE CORRUPTION
echo "Test 5 - Envoi d'une image de 97474 bytes sur un réseau avec un faible taux de corruption (5/100)"
./link_sim -p 8888 -P 9999 -e 5 -s 4269 &
linksim_pid=$!
./receiver -o test5 ::1 9999 &
receiver_pid=$!
./../sender ::1 8888 -f rogue\ one.jpg &
sender_pid=$!

echo "Le sender a terminé son exécution"
wait $sender_pid

if [ $? -eq 0 ]
then
    echo " "
    echo "Test 5 - réussi"
else
    echo " "
    echo "Echec Test 5"
fi

kill -9 $linksim_pid $receiver_pid

# TEST 6 - FAIBLE TAUX DE HEADER CUT
echo "Test 6 - Envoi d'une image de 97474 bytes sur un réseau avec un faible taux de congestion (5/100)"
./link_sim -p 8888 -P 9999 -s 4269 &
linksim_pid=$!
./receiver -o test6 ::1 9999 &
receiver_pid=$!
./../sender ::1 8888 -f rogue\ one.jpg &
sender_pid=$!

echo "Le sender a terminé son exécution"
wait $sender_pid

if [ $? -eq 0 ]
then
    echo " "
    echo "Test 6 - réussi"
else
    echo " "
    echo "Echec Test 6"
fi

kill -9 $linksim_pid $receiver_pid

# TEST 7 - ULTIMATE RÉSEAU EN ANTARTIQUE
echo "Test 7 - Envoi d'une image de 97474 bytes sur un réseau catastrophique, avec un délai de 1000ms, un jitter de 500, une perte de 15/100, de la corruption pour 15/100 et de la congestion pour 15/100"
./link_sim -p 8888 -P 9999 -d 1000 -j 500 -l 15 -e 15 -c 15 -s 4269 &
linksim_pid=$!
./receiver -o test7 ::1 9999 &
receiver_pid=$!
./../sender ::1 8888 -f rogue\ one.jpg &
sender_pid=$!

echo "Le sender a terminé son exécution"
wait $sender_pid

if [ $? -eq 0 ]
then
    echo " "
    echo "Test 7 - réussi"
else
    echo " "
    echo "Echec Test 7"
fi

kill -9 $linksim_pid $receiver_pid

exit 0