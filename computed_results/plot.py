import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

names = [
    #'res_LB07-bunny-sml_2s',
    #'res_LB07-bunny-sml_10s',
    #'res_bone_subxyz_subxy_3s',
    #'res_synth_250000_1049975',
    #'res_res_synth_250000_1049975_7s'
    'res_synth_2'
    ]

stdes = [
    '0.0001',
    '0.001',
    '0.01',
    '0.1',
    '1',
    '4',
    '8',
    '16'
]

plt.rcParams["figure.autolayout"] = True

def plot_mean_and_CI(mean, lb, ub, color_mean=None, color_shading=None, label_name=None, plot_name=None):
    # plot the shaded range of the confidence intervals
    plt.fill_between(range(mean.shape[0]), ub, lb,
                     color=color_shading, alpha=.5)
    # plot the mean on top
    plt.plot(mean, color_mean, label=label_name)

for name in names:
    df = pd.DataFrame()
    df_std = pd.DataFrame()
    for std in stdes:
        #print(std)
        df_read = pd.read_csv(name + '/' + name + '_' + std + '.csv')
        #print(df)
        #print(df_read.mean())
        del df_read[' edges_prep']
        del df_read[' paths_prep']
        del df_read[' paths']
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
    bk = df['boy_kol'].to_numpy()
    bk_std = df['boy_kol'].to_numpy()
    pr = df[' push_rel'].to_numpy()
    pr_std = df[' push_rel'].to_numpy()
    ed = df[' edges'].to_numpy()
    ed_std = df[' edges'].to_numpy()
    fig = plt.figure()
    plot_mean_and_CI(bk, bk+bk_std*0.1, bk-bk_std*0.1, color_mean='b', color_shading='b', label_name='boy_kol', plot_name=name)
    plot_mean_and_CI(pr, pr+pr_std*0.1, pr-pr_std*0.1, color_mean='r', color_shading='r', label_name='push_rel', plot_name=name)
    plot_mean_and_CI(ed, ed+ed_std*0.1, ed-ed_std*0.1, color_mean='g', color_shading='g', label_name='edges', plot_name=name)
    plt.title(name)
    plt.legend()
    #df.set_index('std_mul').plot(title=name, logx=True, ylabel='seconds')
    #df_std.set_index('std_mul').plot(ax=axis[1], title=name, logx=True, ylabel='seconds')


plt.show()