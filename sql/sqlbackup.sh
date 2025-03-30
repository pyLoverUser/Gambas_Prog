#!/bin/bash
mysqldump -u root -pasperg Belegung > "/media/pi/B044-09F6/$(date +\%Y-\%m-\%d-\%h-\%M) Belegung.sql"


