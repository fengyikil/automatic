import torch
from torch import nn
from matplotlib import pyplot as plt
import numpy as np


class Network(nn.Module):
    def __init__(self, n_in, n_hidden, n_out):
        super().__init__()
        self.layer1 = nn.Linear(n_in, n_hidden)
        # self.layer2 = nn.Linear(n_hidden, n_out)

    def forward(self, x):
        x = self.layer1(x)
        # x = torch.sigmoid(x)
       # 创建一个范围作为指数
        exponents = torch.arange(1, x.numel()+ 1).to(x.device)
        y = x.pow(exponents)
        # return self.layer2(y)
        return y.sum()


if __name__ == '__main__':
    x = np.arange(0.0, 2, 0.01)
    y = 100*pow(x,3)
    x = x.reshape(200, 1)
    y = y.reshape(200, 1)
    plt.scatter(x, y)
    x = torch.Tensor(x)
    y = torch.Tensor(y)
    model = Network(1, 6, 1)
    criterion = nn.MSELoss()
    optimizer = torch.optim.Adam(model.parameters(), lr=0.001)

    for epoch in range(100):
        for i in range(0,len(x)):
            y_pred = model(x[i])
            loss = criterion(y_pred, y[i])
            loss.backward()
            optimizer.step()
            optimizer.zero_grad()
        print(f'After {i} iterations, the loss is {loss.item()}')

    # for epoch in range(10000):
    #     y_pred = model(x)
    #     loss = criterion(y_pred, y)
    #     loss.backward()
    #     optimizer.step()
    #     optimizer.zero_grad()

    #     if epoch % 1000 == 0:
    #         print(f'After {epoch} iterations, the loss is {loss.item()}')
    h = torch.zeros_like(x)
    for i in range(1,len(x)+1):
        h[i-1] = model(x[i-1])
    x = x.data.numpy()
    h = h.data.numpy()
    print(model.layer1.weight)
    print(model.layer1.bias)
    # print(model.layer2.weight)
    plt.scatter(x, h)
    plt.show()
