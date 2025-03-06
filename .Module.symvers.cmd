cmd_/home/ubuntu/osproject/Module.symvers := sed 's/\.ko$$/\.o/' /home/ubuntu/osproject/modules.order | scripts/mod/modpost -m -a  -o /home/ubuntu/osproject/Module.symvers -e -i Module.symvers   -T -
