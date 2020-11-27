# From the doc of nodered -> https://nodered.org/docs/getting-started/docker
FROM nodered/node-red

# install custom package
COPY package.json .
RUN npm install --unsafe-perm --no-update-notifier --no-fund --only=production

# load custom settings
COPY settings.js /data/settings.js
COPY flows.json /data/flows.json


