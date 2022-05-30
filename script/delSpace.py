def replaceSpacewithZero(path):
    with open(path, 'r') as f:
        data = f.read()

        print(data)

        filedata = data.replace(' ', '0')

        with open(path, 'w') as file:
            file.write(filedata)
replaceSpacewithZero('/Users/tabowsy/Downloads/SDK/Synopsys_SDK_WEI/Synopsys_SDK/User_Project/HAR_data_collection/userdata/our_dataset.txt')

