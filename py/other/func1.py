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
        # x = torch.sigmoid(x)
        x = torch.relu(x)
        return self.layer2(x)


if __name__ == '__main__':
    x = np.arange(0.0, 10, 0.01)
    x1 = np.arange(0.0, 10, 0.01)
    y = x**2
    y1 = x1**2
    x = x.reshape(1000, 1)
    y = y.reshape(1000, 1)
    y1 = y1.reshape(1000,1)
    x1 = x1.reshape(1000,1)
    plt.scatter(x, y)
    plt.scatter(x1, y1)
    x = torch.Tensor(x)
    y = torch.Tensor(y)
    x1 = torch.Tensor(x1)
    model = Network(1, 8, 1)
    criterion = nn.MSELoss()
    optimizer = torch.optim.Adam(model.parameters(), lr=0.1)
    for epoch in range(5000):
        y_pred = model(x)
        loss = criterion(y_pred, y)
        loss.backward()
        optimizer.step()
        optimizer.zero_grad()

        if epoch % 1000 == 0:
            print(f'After {epoch} iterations, the loss is {loss.item()}')

    print("First Layer Weights:")
    print(model.layer1.weight,model.layer1.bias)
    print("Two Layer Weights:")
    print(model.layer2.weight,model.layer2.bias)
    h = model(x1)
    x1 = x1.data.numpy()
    h = h.data.numpy()
    plt.scatter(x1, h)
    plt.show()
    
