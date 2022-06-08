import pandas as pd
import matplotlib.pyplot as plt

names = [
    'preprocessed_tsukuba.txt',
    'bone_processed.max',
    'LB07-bunny-sml.max'
]

train_samples = [
    '1'
]

test_samples = [
    '5'
]

stdes = [
    '0',
    '0.01',
    '0.1',
    '1',
    '4',
    '10'
]

plt.rcParams["figure.autolayout"] = True

def plot_mean_and_CI(mean, lb, ub, color_mean=None, color_shading=None, label_name=None, plot_name=None, ax=None):
    plt.fill_between(range(mean.shape[0]), ub, lb,
                     color=color_shading, alpha=.5)
    plt.plot(mean, color_mean, label=label_name)
    #print(mean)
    #return mean.plot(x='std_mul', ylabel='seconds', ax=ax)

for name in names:
    df = pd.DataFrame()
    df_std = pd.DataFrame()
    for trs in train_samples:
        for tss in test_samples:
            for std in stdes:
                #print(std)
                df_read = pd.read_csv('results/data/' + name + '_' + trs + '_' + tss + '_' + std + '.csv')
                #print(df)
                #print(df_read.mean())
                df_read = df_read.agg(['mean', 'count', 'std'])
                df = df.append(df_read.loc['mean'], ignore_index=True)
                df_std = df_std.append(df_read.loc['std'], ignore_index=True)
                #plt.plot((df_read.loc['mean']))
    #print(list(map(int, stdes)))
    #print(stdes)
    df['std_mul'] = list(map(float, stdes))
    df_std['std_mul'] = list(map(float, stdes))
    #plt.hist()
    #figure, axis = plt.subplots(1, 2)
    #preflow, add_edge, paths_rem

    bk = df['preflow'].to_numpy()
    bk_std = df_std['preflow'].to_numpy()
    pr = df[' add_edge'].to_numpy()
    pr_std = df_std[' add_edge'].to_numpy()
    ed = df[' paths_rem'].to_numpy()
    ed_std = df_std[' paths_rem'].to_numpy()
    fig = plt.figure()
    plot_mean_and_CI(bk, bk+bk_std*0.1, bk-bk_std*0.1, color_mean='b', color_shading='b', label_name='preflow', plot_name=name)
    plot_mean_and_CI(pr, pr+pr_std*0.1, pr-pr_std*0.1, color_mean='r', color_shading='r', label_name='add_edge', plot_name=name)
    plot_mean_and_CI(ed, ed+ed_std*0.1, ed-ed_std*0.1, color_mean='g', color_shading='g', label_name='paths_rem', plot_name=name)
    plt.xticks([0, 1, 2, 3, 4, 5], stdes)
    plt.title(name)
    plt.legend()
    #df_std.set_index('std_mul').plot(ax=axis[1], title=name, logx=True, ylabel='seconds')


plt.show()
