import torch
from torch import nn
from matplotlib import pyplot as plt
import numpy as np


class Network(nn.Module):
    def __init__(self, n_in, n_hidden, n_out):
        super().__init__()
        self.layer1 = nn.Linear(n_in, n_hidden)
        self.layer2 = nn.Linear(n_hidden, n_out)

    def forward(self, x):
        x = self.layer1(x)
        x = torch.sigmoid(x)
        return self.layer2(x)


if __name__ == '__main__':
    x = np.arange(0.0, 1.0, 0.01)
    y = np.sin(2*np.pi*x)
    x = x.reshape(100, 1)
    y = y.reshape(100, 1)
    plt.scatter(x, y)
    x = torch.Tensor(x)
    y = torch.Tensor(y)
    model = Network(1, 10, 1)
    criterion = nn.MSELoss()
    optimizer = torch.optim.Adam(model.parameters(), lr=0.001)
    for epoch in range(10000):
        y_pred = model(x)
        loss = criterion(y_pred, y)
        loss.backward()
        optimizer.step()
        optimizer.zero_grad()

        if epoch % 1000 == 0:
            print(f'After {epoch} iterations, the loss is {loss.item()}')
    x1 = torch.tensor([0.9,0.8])
    x1 = x1.unsqueeze(1)
    h = model(x1)
    x1 = x1.data.numpy()
    h = h.data.numpy()
    plt.scatter(x1, h)
    plt.show()
