#!/bin/bash

mysqladmin -u root -pasperg -f drop Belegung
mysqladmin -u root -pasperg create Belegung
mysql -u root -pasperg Belegung < $1

sudo systemctl restart mysql

