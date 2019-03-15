#SkyMap

##Introduction
This is a application which can recognize the stars
in picture taken by people. 

###Technology
- Back-end
C++ (Clion)
- Front-end
QT
- Lib
OpenCV

------------------------
##Project Structure
###Class-SkyMapMatching
该类位于系统的第一层，该类的主要任务如下:<br>
- 加载原始的数据集并存储
- 加载并处理图象，将图象中的星抽象成一个个点并存储
- 通过Match方法调用具体的匹配模型
- 从图像中选择一颗星作为匹配目标星
- 存储匹配结果
- 对匹配的结果进行检验
- <br>
- 匹配方法的选择和评价 （未完成）
- 为前端提供接口 （未完成）
- 负责对所有的参数进行检查，保证传入模型的参数是合法的 （未完成）

--------------------------
##Intsall

##Run




