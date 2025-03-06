cmd_/home/ubuntu/osproject/modules.order := {   echo /home/ubuntu/osproject/tm.ko; :; } | awk '!x[$$0]++' - > /home/ubuntu/osproject/modules.order
