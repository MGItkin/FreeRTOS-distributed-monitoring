rm index.zip
cd src
zip -r -X ../index.zip *
cd ..
aws lambda update-function-code --function-name MicroWatchLambda --zip-file fileb://index.zip
