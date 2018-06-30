#! /bin/zsh
gnome-terminal -t "Server" -- bash -c "./server;exec bash;"
gnome-terminal -t "Analyse Pre HTML" -- bash -c "python3 analyse_pre_url.py;exec bash;"
gnome-terminal -t "Analyse User HTML" -- bash -c "python3 analyse_user_data.py;exec bash;"
gnome-terminal -t "Connect Pre URL" -- bash -c "python3 connect_pre_url.py;exec bash;"
gnome-terminal -t "Connect User URL" -- bash -c "python3 connect_user_url.py;exec bash;"
