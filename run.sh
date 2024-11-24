#!/bin/sh

cd scripts
python3 port_inline.py
python3 port_html_inline.py
cd ..
cd backend
make all
cd ..
./backend/bin/alice &
./backend/bin/bob &
open frontend/client_1.html & 
open frontend/client_2.html &
