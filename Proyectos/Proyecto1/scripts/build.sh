docker login

docker build -t maxandr322/proyecto1-redes .

docker images

docker run -it --rm maxandr322/proyecto1-redes 
docker exec -it proyecto1-redes  bash

docker push maxandr322/proyecto1-redes
