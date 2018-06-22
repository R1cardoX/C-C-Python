from PIL import Image
import numpy as np
from scipy.ndimage import filters
import matplotlib.pyplot as plt

'''
#灰度图像高斯模糊
im = np.array(Image.open('./res/2.jpg').convert('L'))
im2 = filters.gaussian_filter(im,5)
plt.gray()
plt.imshow(im)
plt.figure()
plt.imshow(im2)
plt.show()
'''

'''
#彩色图片高斯模糊
im = np.array(Image.open('./res/2.jpg'))
im2 = np.zeros(im.shape)
for i in range(3):
    im2[:,:,i] = filters.gaussian_filter(im[:,:,i],5)
im2 = np.array(im2,'uint8')
plt.imshow(im)
plt.figure()
plt.imshow(im2)
plt.show()
'''

'''
#图像导数正常操作
im = np.array(Image.open('./res/4.jpg').convert('L'))
plt.gray()
plt.imshow(im)
imx = np.zeros(im.shape)
filters.sobel(im,1,imx)
plt.figure()
plt.imshow(imx)
imy = np.zeros(im.shape)
filters.sobel(im,0,imy)
plt.figure()
plt.imshow(imy)
magnitude = np.sqrt(imx**2+imy**2)
plt.figure()
plt.imshow(magnitude)
plt.show()
'''

'''
#图像导数高端操作
sigma = 5 #标准差
im = np.array(Image.open('./res/4.jpg').convert('L'))
plt.gray()
imx = np.zeros(im.shape)
filters.gaussian_filter(im,(sigma,sigma),(0,1),imx)
imy = np.zeros(im.shape)
filters.gaussian_filter(im,(sigma,sigma),(1,0),imy)
magnitude = np.sqrt(imx**2+imy**2)
plt.subplot(221)
plt.imshow(im)
plt.subplot(222)
plt.imshow(magnitude)
plt.subplot(223)
plt.imshow(imx)
plt.subplot(224)
plt.imshow(imy)
plt.show()
'''

def denoise(im,U_init,tolerance=0.1,tau=0.125,tv_weight=100):
    #输入：含有噪声的输入图像，u的初始值，tv的正则项权值，步长，停业条件
    #输出去噪，去纹理的图像。纹理残留
    m,n = im.shape #噪声图像大小
    U = U_init
    Px = im #对偶域x分量
    Py = im #对偶域y分量
    error = 1
    while error > tolerance:
        Uold = U
        #原始变量梯度
        GradUx = np.roll(U,-1,axis=1)-U
        GradUy = np.roll(U,-1,axis=0)-U
        #更新对偶变量
        PxNew = Px + (tau/tv_weight)*GradUx
        PyNew = Py + (tau/tv_weight)*GradUy
        NormNew = np.maximum(1,np.sqrt(PxNew**2+PyNew**2))

        Px = PxNew/NormNew
        Py = PyNew/NormNew

        RxPx = np.roll(Px,1,axis=1)
        RyPy = np.roll(Py,1,axis=0)

        DivP = (Px-RxPx) + (Py-RyPy)
        U = im + tv_weight*DivP

        error = np.linalg.norm(U-Uold)/np.sqrt(n*m)

        return U,im-U


im = np.array(Image.open('./res/2.jpg').convert('L'))
U,T =denoise(im,im)
plt.gray()
plt.imshow(im)
plt.figure()
plt.imshow(U)
plt.axis('equal')
plt.axis('off')
plt.show()

