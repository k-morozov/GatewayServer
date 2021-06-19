# prepare new user
sudo -u postgres psql -c "ALTER USER postgres PASSWORD '123';"
psql -U postgres -h localhost -c "CREATE USER worker with PASSWORD '123';"
psql -U postgres -h localhost -c "CREATE DATABASE chat;"
psql -U postgres -h localhost -c "GRANT ALL PRIVILEGES ON DATABASE chat TO worker;"

# configure Db
psql -U worker -h localhost -d chat "CREATE TABLE clients(id serial primary key, login varchar(32), password varchar(32));"
psql -U worker -h localhost -d chat "CREATE TABLE history(client_id integer references clients(id), channel_name varchar(32), datetime timestamp, message varchar(255));"

psql -U worker -h localhost -d chat "CREATE TABLE channels(id serial primary key, channel_name varchar(32));"
psql -U worker -h localhost -d chat "CREATE TABLE subscriptions(client_id integer, channel_id integer);"