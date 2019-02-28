import urllib.request
# Download the file from `url` and save it locally under `file_name`:
url = 'www.google.it'
file_name = 'pb1.csv'
urllib.request.urlretrieve(url, file_name)
