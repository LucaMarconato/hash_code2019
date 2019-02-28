import urllib.request
# Download the file from `url` and save it locally under `file_name`:
url1 = 'https://hashcodejudge.withgoogle.com/download/blob/AMIfv97QHzUItY7To0o6id8E6_kxpE1KwTlPR1T9yC2zlbm46xvPPX7tBn1r5ZNxw9XboxHkVKa82BzM-B65uM92pJRONqfeVlHfpzH8xbqKc9FrAWJnd3Yp5SF7naqscxmnBjTa3R-UcB_bqg5hoAa0TZFXIlvi-RL6y_daJXyNUUHeP1kBxQss_1b2nNbNFg7W3Jtgyl2Vs86QrufNrQcUlNh754HyhRrhV3yJBo66HdYQAa2lPISjoUkUTpWqqrGIEItpPu27GNqcp2I69D8j8A-70B30oU-w7zIe9DJ9xnpC70T6T88'
file_name = 'a_example.txt'
urllib.request.urlretrieve(url1, file_name)
