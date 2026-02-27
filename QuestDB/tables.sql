-- dev
CREATE TABLE 'solar_dev_measure' (measuretime TIMESTAMP, voltage INT) timestamp (measuretime) PARTITION BY DAY BYPASS WAL;

-- prod
CREATE TABLE 'solar_prod_measure' (measuretime TIMESTAMP, voltage INT) timestamp (measuretime) PARTITION BY DAY BYPASS WAL;
