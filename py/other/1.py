import torch
import matplotlib.pyplot as plt

def test1():
    w = torch.tensor([8.0], requires_grad=True)
    loss = (3 - w[0])**2
    opti = torch.optim.SGD([w], lr=0.01)
    loss.backward()
    print("w的梯度:", w.grad)
    print("w:", w)
    opti.step()
    print("w:", w)

def test11():
    w1 = torch.tensor([8.0], requires_grad=True)
    w2 = torch.tensor([6.0], requires_grad=True)
    loss = (3 - w1[0] + w2[0])**2
    opti1 = torch.optim.SGD([w1], lr=0.01)
    opti2 = torch.optim.SGD([w2], lr=0.01)
    loss.backward()
    print("w1的梯度:", w1.grad)
    print("w2的梯度:", w2.grad)
    print("w1:", w1)
    print("w2:", w2)
    opti1.step()
    opti2.step()
    print("w1:", w1)
    print("w2:", w2)

def test2():
    # 判断是否支持CUDA
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    # 创建一个Tensor
    x = torch.rand(3, 3)
    # 将Tensor转移到GPU上
    x_gpu = x.to(device)
    # 或者
    x_gpu = x.cuda()
    # 将Tensor转移到CPU上
    x_cpu = x_gpu.cpu()

def test3():
    # 创建一个Tensor
    x1 = torch.tensor([1.0, 2.0, 3.0], requires_grad=True)
    x2 = torch.tensor([1.0, 2.0, 3.0], requires_grad=True)
    print(x1.grad)
    print(x2.grad)
    loss = torch.zeros_like(x1)
    #通过循环按位置进行幂次操作 
    for i in range(1, len(x1)+1):
       loss[i-1] = torch.pow(x1[i-1].clone(), i)

    loss = loss.sum()
    loss.backward()
    print(x1.grad)
   
     # 创建一个范围作为指数 
    exponents = torch.arange(1, x2.numel()+ 1).to(x2.device)
    loss = x2.pow(exponents)
    loss = loss.sum()
    loss.backward()
    print(x2.grad)

test3()