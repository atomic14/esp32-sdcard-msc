
# SD Card Performance Directly Connected to PC

|   | Average Speed (B/s) | Average Speed (Mbytes/s) | Average Speed (Mbits/s) |
|---|---------------------|--------------------------|-------------------------|
| **Write** | 27,297,357 | 26.03 | 208.26 |
| **Read**  | 94,395,970 | 90.02 | 720.18 |

Feel free to use this summarized data as needed!

# SPI 20MHz Clock

## Arduino SD Card Code - Single Sector Writing


| Type        | Test 1 (B/s) | Test 2 (B/s) | Test 3 (B/s) | Test 4 (B/s) | Test 5 (B/s) | Average (Mbits/s) | Average (MBytes/s) |
| ----------- | ------------ | ------------ | ------------ | ------------ | ------------ | ----------------- | ------------------ |
| Write Speed | 279717       | 276096       | 279594       | 279577       | 275318       | 2.22              | 0.278              |
| Read Speed  | 506918       | 506782       | 506046       | 506007       | 511699       | 4.06              | 0.507              |


## IDF SD Card Code - Multi Sector Writing


| Type        | Test 1 (B/s) | Test 2 (B/s) | Test 3 (B/s) | Test 4 (B/s) | Test 5 (B/s) | Average (Mbits/s) | Average (MBytes/s) |
| ----------- | ------------ | ------------ | ------------ | ------------ | ------------ | ----------------- | ------------------ |
| Write Speed | 476410       | 481890       | 482435       | 483092       | 481733       | 3.85              | 0.481              |
| Read Speed  | 667243       | 670885       | 671615       | 673308       | 674225       | 5.37              | 0.671              |


## IDF SD Card Code - Multi Sector Lazy Writing

| Type        | Test 1 (B/s) | Test 2 (B/s) | Test 3 (B/s) | Test 4 (B/s) | Test 5 (B/s) | Average (Mbits/s) | Average (MBytes/s) |
| ----------- | ------------ | ------------ | ------------ | ------------ | ------------ | ----------------- | ------------------ |
| Write Speed | 918150       | 915334       | 911385       | 913506       | 915925       | 7.32              | 0.915              |
| Read Speed  | 668170       | 665781       | 673559       | 674341       | 675787       | 5.37              | 0.672              |

## Comparison

![Average Speeds Chart](images/average_speeds_chart.svg)

# Raw SD Card Test Writing 100MB

20MHz SPI Clock

| Operation | Size (MBytes) | Duration (ms) | Speed (Mbits/s) | Speed (MBytes/s) |
| --------- | ------------- | ------------- | --------------- | ---------------- |
| Write     | 100           | 103779        | 8.08            | 1.01             |
| Read      | 100           | 62884         | 13.36           | 1.67             |


# SDIO 4-bit 40MHz Clock

| Operation | Size (MBytes) | Duration (ms) | Speed (Mbits/s)        | Speed (MBytes/s)       |
|-----------|---------------|---------------|------------------------|------------------------|
| Write     | 100.0         | 44828         | 18.71                  | 2.34                   |
| Read      | 100.0         | 12492         | 67.15                  | 8.39                   |

## SDIO 4 bit - Multi Sector Writing

| Type        | Test 1 (B/s) | Test 2 (B/s) | Test 3 (B/s) | Test 4 (B/s) | Test 5 (B/s) | Average (Mbits/s) | Average (MBytes/s) |
| ----------- | ------------ | ------------ | ------------ | ------------ | ------------ | ----------------- | ------------------ |
| Write Speed | 658148       | 655762       | 656099       | 653459       | 657908       | 5.25              | 0.66               |
| Read Speed  | 997525       | 992491       | 1004302      | 1005304      | 1009058      | 8.01              | 1.00               |

## SDIO 4 bit - Multi Sector Lazy Writing

| Type        | Test 1 (B/s) | Test 2 (B/s) | Test 3 (B/s) | Test 4 (B/s) | Test 5 (B/s) | Average (Mbits/s) | Average (MBytes/s) |
| ----------- | ------------ | ------------ | ------------ | ------------ | ------------ | ----------------- | ------------------ |
| Write Speed | 925988       | 921394       | 925877       | 926333       | 923891       | 7.41              | 0.92               |
| Read Speed  | 993128       | 979758       | 1002773      | 1005894      | 1012804      | 7.99              | 1.00               |