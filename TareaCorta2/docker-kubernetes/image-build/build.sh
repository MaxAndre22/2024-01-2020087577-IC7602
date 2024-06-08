sudo docker login

cd ../server-app

sudo docker build -t dockerUser/tc2 .
sudo docker push dockerUser/tc2

cd ..

curl -fsSL -o get_helm.sh https://raw.githubusercontent.com/helm/helm/main/scripts/get-helm-3
chmod 700 get_helm.sh
./get_helm.sh

helm create tc2

rm -rf tc2/templates/*

truncate -s0 tc2/values.yaml

cp image-build/tc2.yaml tc2/templates/

cat image-build/values.yaml > tc2/values.yaml

helm install tc2 ./tc2