#!/bin/bash
mysqldump -u root -pasperg Belegung > /home/pi/Backup/$(date +\%Y-\%m-\%d-\%H-\%M)_Belegung.sql
