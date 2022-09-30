# MyRayTracer
a small RayTracer based on Qt for study Raytracing and c++
## 介绍
- 之前学习了蒙特卡洛方法和重要性采样，为试验加考验自身能力而开的坑
- 一个微型的光线追踪器（目前），文件分类：
    - [x] Vector3向量类
    - [x] Camera相机类
    - [x] Ray类
    - [x] Transform类
    - [x] Material材质类
    - [ ] Shape物体类
       - [x] sphere球类
       - [ ] quad平面类
       - [x] triangle三角形类
       - [x] world世界类
    - [ ] BVH类
- 目标功能：
    - [ ] 渲染
        - [x] 基本图像渲染（击中球体返回颜色）
        - [x] 漫反射
        - [x] 镜面反射
        - [x] 菲涅尔折射
    - [ ] BVH加速
    - [ ] 基于Qt的图像界面
    - [x] 蒙特卡洛积分
    - [x] 重要性采样
## 更新部分
### update by chankkep on 2022/9/21 17:30
1. 实现相机类，球体类，光线类等大体框架
2. 实现光线与球体相交
### update by chankkep on 2022/9/22 16.11
1. 在参考了一下这位大佬的[文章](https://infancy.github.io/smallpt2pbrt.html)和[smallpt](http://www.kevinbeason.com/smallpt/)的代码之后（顺道说一下，大佬的文章真的很通俗易懂，概率论部分让我醍醐灌顶），自己又写了一遍，对渲染方程和BRDF理解更深了，蒙特卡洛和重要性采样还是挺有趣的
2. 先把漫反射写出来了，原渲染方程是这样：
$$L_o(p,\omega_o)=L_{emit}(p,\omega_o) + \int_{H^2(n)=2\pi}f(p,\omega_o,\omega_i)L_{input}(p,\omega_i)cos\theta_id\omega_i$$

漫反射BRDF为：

$$f(p,\omega_o,\omega_i) = \frac1\pi$$

漫反射的渲染方程形式变为如下：

$$L_o(p,\omega_o)=L_{emit}(p,\omega_o) + \int_{H^2}\frac{1}{\pi}L_{input}(p,\omega_i)cos\theta_id\omega_i$$

用蒙特卡洛方法变换一下：

$$L_o(p,\omega_o)=L_{emit}(p,\omega_o) + \lim_{N\to\infty}\sum^{N}_{i=1}(\frac{1}{\pi}L_{input}(p,\omega_i)cos\theta_i\Delta\omega_i)$$

$$L_o(p,\omega_o)\approx L_{emit}(p,\omega_o) +2\pi * \frac1N \sum^{N}_{i=1}(\frac{1}{\pi}L_{input}(p,\omega_i)cos\theta_i)$$

对应入射光的Radiance为：

$$\frac{cos\theta_i}{\pi}L_{input}(p,\omega_i)$$

实际代码中，每次往往只会在半球方向中采样一根光线,原因是在不考虑能量损耗的情况下，相机发射的光线可以在一个封闭空间中不停的穿梭，对应到这个方程中就是它会一直递归下去。最简单的办法是达到一定的递归深度就终止，也就是只允许**一条光线**在空间中穿梭有限次。在之前的例子中，其实没有去考虑每个样本可能出现的概率，都是按均等概率抽取样本的，2$\pi$就是采样的总数，可以更改采样的策略，先将N取1，2$\pi$替换为下文要提到的pdf函数

$$L_o(p,\omega_o)\approx L_{emit}(p,\omega_o) + \frac{f(p,\omega_o,\omega_i)L_{input}(p,\omega_i)cos\theta_i}{pdf(x)}$$

如果对该BRDF进行采样那跟均匀采样还是一样的没有区别，我们要的是能更快逼近真实积分值的重要性采样，在这里对 $cos\theta_i$ 采样
- 蒙特卡洛法的核心，是根据某一概率分布来随机采样，那么，划重点，**根据被积函数曲线规律来设计类似的概率密度**来进行采样，这就符合蒙特卡洛积分的要求，也就是重要性采样的核心。
- 所以我们要创建一个我们想要的接近原函数的**概率密度函数** $pdf(x)$ ,这里新建了和余弦函数成比例的一个函数：
- 
$$pdf(x)=c *cos\theta_i$$

因为概率密度函数在区间内的定积分值总为1，故有：

$$\int c*cos\theta_id\omega = 1$$

求解得到 

$$c=\frac1\pi$$

所以

$$pdf(x)=\frac {cos\theta_i}\pi$$

原方程就变为

$$L_o(p,\omega_o)\approx L_{emit}(p,\omega_o) + \frac{f(p,\omega_o,\omega_i)L_{input}(p,\omega_i)cos\theta_i}{\frac{cos\theta_i}\pi}$$

$$L_o(p,\omega_o)\approx L_{emit}(p,\omega_o) + \frac{\frac1\pi L_{input}(p,\omega_i)cos\theta_i}{\frac{cos\theta_i}\pi}$$

最终得到重要性采样漫反射的BRDF渲染方程（albedo为表面反照率，也就是表面颜色）
$$L_o(p,\omega_o)\approx L_{emit}(p,\omega_o) + albedo * L_{input}(p,\omega_i)$$
代码实现就简单了
```c++
Lo = emission + albedo * Radiance(ray(intersect_pos, refl_dir));
```
效果实现如图，spp为400
![file](https://github.com/TheChanCell/MyRayTracer/blob/master/MyRaytracer/image/specular_BRDF.jpg)

### update by chankkep on 2022/9/23 23:31
- 追加镜面反射specularReflection还有镜面透射specularTransmission，主要是跟着[pbrt](https://www.pbr-book.org/3ed-2018/Reflection_Models/Specular_Reflection_and_Transmission#SpecularTransmission)还有上次那个大佬的文章写
- 写镜面透射的时候真的调试了好久，原因还是自己的数学功底不过关
- 这次也做一下笔记：

在折射中，我们可以用**snell定律**求得出射光方向,已知**入射光与表面法线的角度 $\theta_i$ , $\eta_i$ 和 $\eta_t$ 为入射面上方和下方的折射率，求出射光方向与法线之间的角度 $\theta_t$** （此时法线应该是与表面法线方向相反的反法线）

snell定律：
$$\frac{sin\theta_i}{sin\theta_t}=\frac{\eta_t}{\eta_i}$$
表示的意思如图：

![snell](https://github.com/TheChanCell/MyRayTracer/blob/master/MyRaytracer/image/snell.png)

这里设入射、出射光方向分别为 $\omega_i$，$\omega_t$。$\omega_i$已知，根据已知条件分别计算$\omega_t$的横竖分量和平行分量再求和，这里直接贴图：

![txy](https://github.com/TheChanCell/MyRayTracer/blob/master/MyRaytracer/image/txy.png)

代码贴上来：
```c++
    Vector3 refl_dir = ray.direction - 2 * ray.direction.Dot(inf.normal) * inf.normal;

//snell`s law: sinI / sinT = theta_T / theta_I
    double theta_i = 1.0;
    double theta_t = 1.5;

    double eta_frac = inf.outface ? theta_i / theta_t : theta_t / theta_i;

    double cos_etaI = inf.normal.Dot(-ray.direction);
    double sin_etaT2 = std::max(0., 1. - cos_etaI * cos_etaI);
    sin_etaT2 *= eta_frac * eta_frac;
    if(sin_etaT2 >= 1)
        return inf.emission + inf.albedo * Radiance(Ray(inf.position, refl_dir), s, depth);
    double cos_etaT = std::sqrt(1 - sin_etaT2);
    Vector3 refractDir = ray.direction * eta_frac + (eta_frac * cos_etaI - cos_etaT) * inf.normal;
```
然后就是菲涅尔折射，这里我们用[Schlick近似](https://en.wikipedia.org/wiki/Schlick%27s_approximation)

![fresnel](https://github.com/TheChanCell/MyRayTracer/blob/master/MyRaytracer/image/Fresnel.png)

其中n1,n2是界面处两种介质的折射率, $\theta$是入射光的入射方向与表面的法线之间的角度
```c++
double R0 = (theta_t - theta_i) / (theta_t + theta_i);
    R0 *= R0;

//Re是该表面反射的比例
double Re = R0 + (1 - R0) * pow(1 - (inf.outface ? cos_etaI : cos_etaT), 5);
//Tr是折射的比例
double Tr = 1 - Re;
```
然后就是重要性采样和路径追踪
```c++
double P = .25 + .5 * R_theta; //这个不知道为啥要这样，smallpt里是这样，下次改成pbrt里的
double RP = R_theta / P; //Reflect PDF
double TP = Tr / (1 - P); //Refract PDF

color Li;
if(depth > 2)
{
    //Russian roulette
    if(random.random_double01() < P)
    {
        Li = Radiance(Ray(inf.position, refl_dir), s, depth) * RP;
    }
    else
    {
        Li = Radiance(Ray(inf.position, refractDir), s, depth) * TP;
    }
}
else
{
    Li = Radiance(Ray(inf.position, refl_dir), s, depth) * R_theta + Radiance(Ray(inf.position, refractDir), s, depth) * Tr;
}
return inf.emission + inf.albedo * Li;
```
效果图

![image](https://github.com/TheChanCell/MyRayTracer/blob/master/MyRaytracer/image/Fresnel.jpg)

收工！睡觉
### update by chankkep on 2022/9/28 12:01
- 增加triangle类，不知道为什么它的表面辐射出来的Radiance很暗
- 更改并行库为inter的TBB

### update by chankkep on 2022/9/30 16:05
- 把框架改了一遍,现在有个基本的框架,Transform类代表物体的位置、角度、缩放大小，还有一个父Tranform对象，一般是从世界继承而来
- 新添加的Material类是由之前的emission、albedo、materialtype整合在一块，可以使用shared_ptr创建多个材质实例供多个shape使用
- 把采样和渲染的逻辑整合在sampler类里，main函数看起来清爽一些，管理起来也方便。
### TODO:
- Qt图形界面
- 三角形求交算法感觉不对劲，还是先平面求交再求交三角形把
- BVH加速结构

