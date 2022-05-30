import numpy as np 
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
from matplotlib.pyplot import figure

user = 42

# reading raw data file
columns = ['user','activity','timestamp', 'x-axis', 'y-axis', 'z-axis']
df_har = pd.read_csv('userdata/our_dataset.txt', header = None, names = columns)

# removing null values
df_har = df_har.dropna()
# transforming the z-axis to float
df_har['z-axis'] = df_har['z-axis'].str.replace(';', '')
df_har['z-axis'] = df_har['z-axis'].apply(lambda x:float(x))
# drop rows where timestamp is 0
df = df_har[df_har['timestamp'] != 0]
# arrange data in ascending order of user and timestamp
df = df.sort_values(by = ['user', 'timestamp'], ignore_index=True)


figure(figsize=(6, 5), dpi=80)
sns.set_style('whitegrid')
sns.countplot(x = 'activity', data = df)
plt.title('Number of samples by activity')
plt.savefig('fig/dataset_count.png')

print(df['activity'].value_counts())

activities=['Walking','Jogging','Upstairs','Downstairs','Sitting','Standing']
plt.figure(figsize=(12, 10))
for index,i in enumerate(activities):
  plt.subplot(2,3,index+1)
  data36=df[(df['user']==user)&(df['activity']==i)][100:200]
  sns.lineplot(y='x-axis',x='timestamp',data=data36)
  sns.lineplot(y='y-axis',x='timestamp',data=data36)
  sns.lineplot(y='z-axis',x='timestamp',data=data36)
  plt.legend(['x-axis','y-axis','z-axis'])
  plt.ylabel(i)
  plt.title(i,fontsize=15)
  #plt.savefig(i+'.png')
plt.savefig('fig/'+str(user)+'raw.png')

# copy the data
df_max_scaled = df.copy()
# apply normalization techniques
for ind in df_max_scaled.columns:
  if ind == 'x-axis' or ind == 'y-axis' or ind == 'z-axis':
    df_max_scaled[ind] = (df_max_scaled[ind] + 4) / 8
# indicate 
activities=['Walking','Jogging','Upstairs','Downstairs','Sitting','Standing']
plt.figure(figsize=(12, 10))
for index,i in enumerate(activities):
  plt.subplot(2,3,index+1)
  data36=df_max_scaled[(df_max_scaled['user']==user)&(df_max_scaled['activity']==i)][100:200]
  sns.lineplot(y='x-axis',x='timestamp',data=data36)
  sns.lineplot(y='y-axis',x='timestamp',data=data36)
  sns.lineplot(y='z-axis',x='timestamp',data=data36)
  plt.legend(['x-axis','y-axis','z-axis'])
  plt.ylabel(i)
  plt.title(i,fontsize=15)
plt.savefig('fig/'+str(user)+'_normalized.png')