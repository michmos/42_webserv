#!/bin/bash

RED="\e[31m"
GRE="\e[32m"
YEL="\e[33m"
RES="\e[0m"

echo -e "${YEL} GET localhost:8080: ${RES}"
curl -s --http1.1 -G localhost:8080 -i -o test_results.txt
head -n 1 test_results.txt | grep -q "200" && echo -e "${GRE} Status is 200 OK${RES}" || echo -e "${RED}Status is not ok${RES}"
echo ""

echo -e "${YEL} GET localhost:8080/index.html: ${RES} "
curl -s --http1.1 -G localhost:8080/index.html -i -o test_results.txt
head -n 1 test_results.txt | grep -q "200" && echo -e "${GRE} Status is 200 OK${RES}" || echo -e "${RED}Status is not ok${RES}"
echo ""

echo -e "${YEL} GET localhost:8080/contact.html: ${RES} "
curl -s --http1.1 -G localhost:8080/contact.html -i -o test_results.txt
head -n 1 test_results.txt | grep -q "200" && echo -e "${GRE} Status is 200 OK${RES}" || echo -e "${RED}Status is not ok${RES}"
echo ""

echo -e "${YEL} GET localhost:8080/upload.html: ${RES} "
curl -s --http1.1 -G localhost:8080/upload.html -i -o test_results.txt
head -n 1 test_results.txt | grep -q "200" && echo -e "${GRE} Status is 200 OK${RES}" || echo -e "${RED}Status is not ok${RES}"
echo ""

echo -e "${YEL} CGI upload txt localhost:8080/nph_CGI_upload.py: ${RES} "
curl -s --http1.1 -F "name=test; filename=text.txt" -H "Content-Type: multipart/form-data" localhost:8080/nph_CGI_upload.py -i -o test_results.txt
head -n 1 test_results.txt | grep -q "302" && echo -e "${GRE} Status is 302 found" || echo -e "${RED}Status is not ok${RES}"
echo ""

echo -e "${YEL} CGI upload png localhost:8080/nph_CGI_upload.py: ${RES} "
curl -s --http1.1 -F "name=test2; filename=small.png" -H "Content-Type: multipart/form-data" localhost:8080/nph_CGI_upload.py -i -o test_results.txt
head -n 1 test_results.txt | grep -q "302" && echo -e "${GRE} Status is 302 found" || echo -e "${RED}Status is not ok${RES}"
echo ""

echo -e "${YEL} CGI contact localhost:8080/nph_CGI_submit_contact_info.py: ${RES} "
curl -s --http1.1 -X POST -d "name=Testname Doe&email=test@email.com&message=Hello!" localhost:8080/nph_CGI_submit_contact_info.py -i -o test_results.txt
head -n 1 test_results.txt | grep -q "302" && echo -e "${GRE} Status is 302 found" || echo -e "${RED}Status is not ok${RES}"
echo ""

echo -e "${YEL} DELETE localhost:8080/text.txt: ${RES} "
curl -s --http1.1 -X DELETE localhost:8080/text.txt -i -o test_results.txt
head -n 1 test_results.txt | grep -q "302" && echo -e "${GRE} Status is 302 found" || echo -e "${RED}Status is not ok${RES}"
echo ""

echo -e "${YEL} try DELETE again localhost:8080/text.txt: ${RES} "
curl -s --http1.1 -X DELETE localhost:8080/text.txt -i -o test_results.txt
head -n 1 test_results.txt | grep -q "404" && echo -e "${GRE} Status is 404 OK (not found)" || echo -e "${RED}Status is not ok${RES}"
