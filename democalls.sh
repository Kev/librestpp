#!/bin/bash

# Copyright (c) 2015 Isode Limited.
# All rights reserved.
# See the LICENSE file for more information.


SERVER='http://localhost:1080'
VERBOSE='' # -v

echo "GET /demo"
curl ${VERBOSE} ${SERVER}/demo
echo " "
echo " "

DATA="This is the submitted text"
echo "POST /echotext $DATA" 
curl ${VERBOSE} --data $DATA -X POST ${SERVER}/echotext
echo " "
echo " "

echo "GET /echo"
curl ${VERBOSE} ${SERVER}/echo
echo " "
echo " "

DATA='{"this":"was","submitted":true}'
echo "POST /echo $DATA"
curl ${VERBOSE} --data $DATA -X POST ${SERVER}/echo
echo " "
echo " "

DATA='{"these":"are","more":{"submitted":"data","ok":true}}'
echo "POST /chef $DATA"
curl ${VERBOSE} --data $DATA -X POST ${SERVER}/chef
echo " "
echo " "
