# MyRayTracer
a small RayTracer based on Qt for study Raytracing and c++
## ����
- ֮ǰѧϰ�����ؿ��巽������Ҫ�Բ�����Ϊ����ӿ����������������Ŀ�
- һ��΢�͵Ĺ���׷������Ŀǰ�����ļ����ࣺ
    - [x] Vector3������
    - [x] Camera�����
    - [x] Ray��
    - [x] Transform��
    - [x] Material������
    - [ ] Shape������
       - [x] sphere����
       - [ ] quadƽ����
       - [x] triangle��������
       - [x] world������
    - [ ] BVH��
- Ŀ�깦�ܣ�
    - [ ] ��Ⱦ
        - [x] ����ͼ����Ⱦ���������巵����ɫ��
        - [x] ������
        - [x] ���淴��
        - [x] ����������
    - [ ] BVH����
    - [ ] ����Qt��ͼ�����
    - [x] ���ؿ������
    - [x] ��Ҫ�Բ���
## ���²���
### update by chankkep on 2022/9/21 17:30
1. ʵ������࣬�����࣬������ȴ�����
2. ʵ�ֹ����������ཻ
### update by chankkep on 2022/9/22 16.11
1. �ڲο���һ����λ���е�[����](https://infancy.github.io/smallpt2pbrt.html)��[smallpt](http://www.kevinbeason.com/smallpt/)�Ĵ���֮��˳��˵һ�£����е�������ĺ�ͨ���׶��������۲������������ඥ�����Լ���д��һ�飬����Ⱦ���̺�BRDF�������ˣ����ؿ������Ҫ�Բ�������ͦ��Ȥ��
2. �Ȱ�������д�����ˣ�ԭ��Ⱦ������������
$$L_o(p,\omega_o)=L_{emit}(p,\omega_o) + \int_{H^2(n)=2\pi}f(p,\omega_o,\omega_i)L_{input}(p,\omega_i)cos\theta_id\omega_i$$

������BRDFΪ��

$$f(p,\omega_o,\omega_i) = \frac1\pi$$

���������Ⱦ������ʽ��Ϊ���£�

$$L_o(p,\omega_o)=L_{emit}(p,\omega_o) + \int_{H^2}\frac{1}{\pi}L_{input}(p,\omega_i)cos\theta_id\omega_i$$

�����ؿ��巽���任һ�£�

$$L_o(p,\omega_o)=L_{emit}(p,\omega_o) + \lim_{N\to\infty}\sum^{N}_{i=1}(\frac{1}{\pi}L_{input}(p,\omega_i)cos\theta_i\Delta\omega_i)$$

$$L_o(p,\omega_o)\approx L_{emit}(p,\omega_o) +2\pi * \frac1N \sum^{N}_{i=1}(\frac{1}{\pi}L_{input}(p,\omega_i)cos\theta_i)$$

��Ӧ������RadianceΪ��

$$\frac{cos\theta_i}{\pi}L_{input}(p,\omega_i)$$

ʵ�ʴ����У�ÿ������ֻ���ڰ������в���һ������,ԭ�����ڲ�����������ĵ�����£��������Ĺ��߿�����һ����տռ��в�ͣ�Ĵ��󣬶�Ӧ����������о�������һֱ�ݹ���ȥ����򵥵İ취�Ǵﵽһ���ĵݹ���Ⱦ���ֹ��Ҳ����ֻ����**һ������**�ڿռ��д������޴Ρ���֮ǰ�������У���ʵû��ȥ����ÿ���������ܳ��ֵĸ��ʣ����ǰ����ȸ��ʳ�ȡ�����ģ�2$\pi$���ǲ��������������Ը��Ĳ����Ĳ��ԣ��Ƚ�Nȡ1��2$\pi$�滻Ϊ����Ҫ�ᵽ��pdf����

$$L_o(p,\omega_o)\approx L_{emit}(p,\omega_o) + \frac{f(p,\omega_o,\omega_i)L_{input}(p,\omega_i)cos\theta_i}{pdf(x)}$$

����Ը�BRDF���в����Ǹ����Ȳ�������һ����û����������Ҫ�����ܸ���ƽ���ʵ����ֵ����Ҫ�Բ������������ $cos\theta_i$ ����
- ���ؿ��巨�ĺ��ģ��Ǹ���ĳһ���ʷֲ��������������ô�����ص㣬**���ݱ����������߹�����������Ƶĸ����ܶ�**�����в�������ͷ������ؿ�����ֵ�Ҫ��Ҳ������Ҫ�Բ����ĺ��ġ�
- ��������Ҫ����һ��������Ҫ�Ľӽ�ԭ������**�����ܶȺ���** $pdf(x)$ ,�����½��˺����Һ����ɱ�����һ��������
- 
$$pdf(x)=c *cos\theta_i$$

��Ϊ�����ܶȺ����������ڵĶ�����ֵ��Ϊ1�����У�

$$\int c*cos\theta_id\omega = 1$$

���õ� 

$$c=\frac1\pi$$

����

$$pdf(x)=\frac {cos\theta_i}\pi$$

ԭ���̾ͱ�Ϊ

$$L_o(p,\omega_o)\approx L_{emit}(p,\omega_o) + \frac{f(p,\omega_o,\omega_i)L_{input}(p,\omega_i)cos\theta_i}{\frac{cos\theta_i}\pi}$$

$$L_o(p,\omega_o)\approx L_{emit}(p,\omega_o) + \frac{\frac1\pi L_{input}(p,\omega_i)cos\theta_i}{\frac{cos\theta_i}\pi}$$

���յõ���Ҫ�Բ����������BRDF��Ⱦ���̣�albedoΪ���淴���ʣ�Ҳ���Ǳ�����ɫ��
$$L_o(p,\omega_o)\approx L_{emit}(p,\omega_o) + albedo * L_{input}(p,\omega_i)$$
����ʵ�־ͼ���
```c++
Lo = emission + albedo * Radiance(ray(intersect_pos, refl_dir));
```
Ч��ʵ����ͼ��sppΪ400
![file](https://github.com/TheChanCell/MyRayTracer/blob/master/MyRaytracer/image/specular_BRDF.jpg)

### update by chankkep on 2022/9/23 23:31
- ׷�Ӿ��淴��specularReflection���о���͸��specularTransmission����Ҫ�Ǹ���[pbrt](https://www.pbr-book.org/3ed-2018/Reflection_Models/Specular_Reflection_and_Transmission#SpecularTransmission)�����ϴ��Ǹ����е�����д
- д����͸���ʱ����ĵ����˺þã�ԭ�����Լ�����ѧ���ײ�����
- ���Ҳ��һ�±ʼǣ�

�������У����ǿ�����**snell����**��ó���ⷽ��,��֪**���������淨�ߵĽǶ� $\theta_i$ , $\eta_i$ �� $\eta_t$ Ϊ�������Ϸ����·��������ʣ������ⷽ���뷨��֮��ĽǶ� $\theta_t$** ����ʱ����Ӧ��������淨�߷����෴�ķ����ߣ�

snell���ɣ�
$$\frac{sin\theta_i}{sin\theta_t}=\frac{\eta_t}{\eta_i}$$
��ʾ����˼��ͼ��

![snell](https://github.com/TheChanCell/MyRayTracer/blob/master/MyRaytracer/image/snell.png)

���������䡢����ⷽ��ֱ�Ϊ $\omega_i$��$\omega_t$��$\omega_i$��֪��������֪�����ֱ����$\omega_t$�ĺ���������ƽ�з�������ͣ�����ֱ����ͼ��

![txy](https://github.com/TheChanCell/MyRayTracer/blob/master/MyRaytracer/image/txy.png)

������������
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
Ȼ����Ƿ��������䣬����������[Schlick����](https://en.wikipedia.org/wiki/Schlick%27s_approximation)

![fresnel](https://github.com/TheChanCell/MyRayTracer/blob/master/MyRaytracer/image/Fresnel.png)

����n1,n2�ǽ��洦���ֽ��ʵ�������, $\theta$�����������䷽�������ķ���֮��ĽǶ�
```c++
double R0 = (theta_t - theta_i) / (theta_t + theta_i);
    R0 *= R0;

//Re�Ǹñ��淴��ı���
double Re = R0 + (1 - R0) * pow(1 - (inf.outface ? cos_etaI : cos_etaT), 5);
//Tr������ı���
double Tr = 1 - Re;
```
Ȼ�������Ҫ�Բ�����·��׷��
```c++
double P = .25 + .5 * R_theta; //�����֪��ΪɶҪ������smallpt�����������´θĳ�pbrt���
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
Ч��ͼ

![image](https://github.com/TheChanCell/MyRayTracer/blob/master/MyRaytracer/image/Fresnel.jpg)

�չ���˯��
### update by chankkep on 2022/9/28 12:01
- ����triangle�࣬��֪��Ϊʲô���ı�����������Radiance�ܰ�
- ���Ĳ��п�Ϊinter��TBB

### update by chankkep on 2022/9/30 16:05
- �ѿ�ܸ���һ��,�����и������Ŀ��,Transform����������λ�á��Ƕȡ����Ŵ�С������һ����Tranform����һ���Ǵ�����̳ж���
- ����ӵ�Material������֮ǰ��emission��albedo��materialtype������һ�飬����ʹ��shared_ptr�����������ʵ�������shapeʹ��
- �Ѳ�������Ⱦ���߼�������sampler���main������������ˬһЩ����������Ҳ���㡣
### TODO:
- Qtͼ�ν���
- ���������㷨�о����Ծ���������ƽ�������������ΰ�
- BVH���ٽṹ

