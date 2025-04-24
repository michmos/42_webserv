echo "" > test_results.txt
echo -e "TEST: ${YEL} Stresstest with siege -r 1000 -c 1 http://localhost:$PORT/empty.html: ${RES}"
siege -r 1000 -c 1 http://localhost:$PORT/empty.html
echo ""