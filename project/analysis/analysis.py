import pandas as pd
from plotly import graph_objects as go


def big():

    df = pd.read_csv('N500000000.csv')
    df = df.groupby(['D', 'S', 'T']).median().reset_index()
    def speedup(r):
        v = df[(df['T'] == 1) & (df['D'] == r['D']) & (df['S'] == r['S'])]['t']
        return v.values[0] / r['t']
    df['s'] = df.apply(speedup, axis=1)

    D = 3
    DIST = 'Reversed List'
    df = df.groupby('D').get_group(D)

    xs = [1, 2, 4, 8, 16, 32]

    t0 = go.Scatter(x=list(range(6)), y=xs, name='Ideal', line={'dash': 'dash'})
    t1 = go.Scatter(x=list(range(6)), y=df[df['S'] == 1]['s'], name='Strategy 1')
    t2 = go.Scatter(x=list(range(6)), y=df[df['S'] == 2]['s'], name='Strategy 2')
    t3 = go.Scatter(x=list(range(6)), y=df[df['S'] == 3]['s'], name='Strategy 3')

    layout = go.Layout(title=f'Speedups for {DIST}', xaxis={'title': 'T'}, yaxis={'title': 'speedup'}, legend={'font': {'size': 32}}, font={'size': 32})

    fig = go.Figure()
    fig.add_trace(t0)
    fig.add_trace(t1)
    fig.add_trace(t2)
    fig.add_trace(t3)
    fig.update_layout(layout)
    fig.update_yaxes(tickvals=list(range(0, 33, 2)), tickfont={'size': 24}, titlefont={'size': 32})
    fig.update_xaxes(tickvals=list(range(6)), ticktext=list(map(str, xs)), tickfont={'size': 24}, titlefont={'size': 32})

    fig.show()


def fN():
    # N,t
    df = pd.read_csv('fN.csv')
    df = df.groupby(['N']).median().reset_index()
    
    xs = [1, 2, 4, 8, 16, 32]

    t0 = go.Scatter(x=list(range(6)), y=xs, name='Ideal', line={'dash': 'dash'})
    t1 = go.Scatter(x=list(range(6)), y=df[df['S'] == 1]['s'], name='Strategy 1')
    t2 = go.Scatter(x=list(range(6)), y=df[df['S'] == 2]['s'], name='Strategy 2')
    t3 = go.Scatter(x=list(range(6)), y=df[df['S'] == 3]['s'], name='Strategy 3')

    layout = go.Layout(title=f'Speedups for {DIST}', xaxis={'title': 'T'}, yaxis={'title': 'speedup'}, legend={'font': {'size': 32}}, font={'size': 32})

    fig = go.Figure()
    fig.add_trace(t0)
    fig.add_trace(t1)
    fig.add_trace(t2)
    fig.add_trace(t3)
    fig.update_layout(layout)
    fig.update_yaxes(tickvals=list(range(0, 33, 2)), tickfont={'size': 24}, titlefont={'size': 32})
    fig.update_xaxes(tickvals=list(range(6)), ticktext=list(map(str, xs)), tickfont={'size': 24}, titlefont={'size': 32})

    fig.show()


def strong():
    
    # distribution 1, strategy 2, N 500000000

    df = pd.read_csv('strong.csv')
    df = df.groupby(['T']).median().reset_index()
    def speedup(r):
        v = df[(df['T'] == 1)]['t']
        return v.values[0] / r['t']
    df['s'] = df.apply(speedup, axis=1)

    xs = [1, 2, 4, 8, 16, 32, 64, 128, 256]

    t0 = go.Scatter(x=list(range(6)), y=xs[:6], name='Ideal', line={'dash': 'dash'})
    t1 = go.Scatter(x=list(range(9)), y=df['s'], name='Strategy 2')

    layout = go.Layout(title=f'Strong Scalability with Random Int Distribution', xaxis={'title': 'T'}, yaxis={'title': 'speedup'}, legend={'font': {'size': 32}}, font={'size': 32})

    fig = go.Figure()
    fig.add_trace(t0)
    fig.add_trace(t1)
    fig.update_layout(layout)
    fig.update_yaxes(tickvals=list(range(0, 33, 2)), tickfont={'size': 24}, titlefont={'size': 32})
    fig.update_xaxes(tickvals=list(range(9)), ticktext=list(map(str, xs)), tickfont={'size': 24}, titlefont={'size': 32})

    fig.show()


def weak():
    # distribution 1, strategy 2, N = T * 10^7
    # T. t
    df = pd.read_csv('weak.csv')
    df = df.groupby(['T']).median().reset_index()
    def speedup(r):
        v = df[(df['T'] == 1)]['t']
        return v.values[0] / r['t']
    df['s'] = df.apply(speedup, axis=1)

    xs = [1, 2, 4, 8, 16, 32]

    t0 = go.Scatter(x=list(range(6)), y=[1, 1, 1, 1, 1, 1], name='Ideal', line={'dash': 'dash'})
    t1 = go.Scatter(x=list(range(6)), y=df['s'], name='Strategy 2')

    layout = go.Layout(title=f'Weak Scalability with Random Int Distribution', xaxis={'title': 'T'}, yaxis={'title': 'speedup'}, legend={'font': {'size': 32}}, font={'size': 32})

    fig = go.Figure()
    fig.add_trace(t0)
    fig.add_trace(t1)
    fig.update_layout(layout)
    fig.update_yaxes(tickmode='linear', dtick=0.1, range=[0, 1.1], tickfont={'size': 24}, titlefont={'size': 32})
    fig.update_xaxes(tickvals=list(range(6)), ticktext=list(map(str, xs)), tickfont={'size': 24}, titlefont={'size': 32})

    fig.show()

if __name__ == '__main__':
    weak()
