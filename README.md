# QT词法语法分析

​	本学期上过编译原理的课程，课程设计要求做一个词法语法分析器，就相当于一个编译器的部分功能，正好最近也在学习QT，所以就用QT做课设，里面涉及到**sqlite数据库对种别码表信息进行增删查改，另外有修改字体，颜色，背景进行永久保存**，相关源代码在最下面，可以选择github下载链接，不需要积分。

[TOC]

------



## 词法分析

​	词法分析是计算机科学中将字符序列转换为单词序列的过程。进行词法分析的程序或者函数叫作词法分析器，也叫扫描器。词法分析器一般以函数的形式存在，供语法分析器调用。

​	词法分析是编译程序的第一个阶段且是必要阶段；词法分析的核心任务是扫描、识别单词且对识别出的单词给出定性、定长的处理；实现词法分析程序的常用途径:自动生成,手工生成。

**1.单词种别构成表**

|         单词符号         | 种别码 | 单词符号 | 种别码 |
| :----------------------: | :----: | :------: | :----: |
|          begin           |   1    |    ：    |   17   |
|            if            |   2    |   ：=    |   18   |
|           then           |   3    |    <     |   20   |
|          while           |   4    |    <>    |   21   |
|            do            |   5    |    <=    |   22   |
|           end            |   6    |    >     |   23   |
| lettet（letter\|digit）* |   10   |    >=    |   24   |
|      dight  dight*       |   11   |    =     |   25   |
|            +             |   13   |    ；    |   26   |
|            —             |   14   |    (     |   27   |
|            *             |   15   |    )     |   28   |
|            /             |   16   |    #     |   0    |

**2.状态转换图**

![在这里插入图片描述](https://img-blog.csdnimg.cn/20200915143326221.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQyNzEyODE2,size_16,color_FFFFFF,t_70#pic_center)


## 语法分析

	递归下降语法分析是在每个非终结符都对应一个子程序。该子程序根据下一个输入符号来确定按照哪一个产生式进行处理，再根据该产生式的右端:

1.每遇到一个终结符，则判断当前读入的单词是否与该终结符相匹配，若匹配，再读取下一个单词继续分析；不匹配，则进行出错处理

2.每遇到一个非终结符，则调用相应的子程序

 

语法分析器在其工作过程中，一般应完成下列的任务：

（1）给出语法分析的文法；

（2）有必要还需要将文法转成LL（1）文法；

（3）求出first和follow集合构造预测分析表；

（4）进行语法检查，报告所发现的错误。



## 相关流程图

### **词法分析流程图**
![在这里插入图片描述](https://img-blog.csdnimg.cn/20200915143002630.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQyNzEyODE2,size_16,color_FFFFFF,t_70#pic_center)

### **递归下降分析流程图**
![在这里插入图片描述](https://img-blog.csdnimg.cn/20200915143028998.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQyNzEyODE2,size_16,color_FFFFFF,t_70#pic_center)

### **语句串分析流程图**
![在这里插入图片描述](https://img-blog.csdnimg.cn/20200915143048181.jpg#pic_center)

### **语句分析流程图**

![在这里插入图片描述](https://img-blog.csdnimg.cn/20200915143126444.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQyNzEyODE2,size_16,color_FFFFFF,t_70#pic_center)


### **表达式分析流程图**

![在这里插入图片描述](https://img-blog.csdnimg.cn/20200915143137340.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQyNzEyODE2,size_16,color_FFFFFF,t_70#pic_center)


### **项分析流程图**

![在这里插入图片描述](https://img-blog.csdnimg.cn/20200915143147634.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQyNzEyODE2,size_16,color_FFFFFF,t_70#pic_center)


### **因子分析流程图**

![在这里插入图片描述](https://img-blog.csdnimg.cn/2020091514315664.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQyNzEyODE2,size_16,color_FFFFFF,t_70#pic_center)

## 功能演示

### 词法分析

	可识别符号对应的种别码，未识别的种别码也会显示在错误结果框中，可以将需要识别的种别码加入到种别码表中，这样词法分析就可以识别。

![在这里插入图片描述](https://img-blog.csdnimg.cn/20200915143348834.gif#pic_center)

### 语法分析

可以识别语句错误，缺少括号，if语句判断。

![在这里插入图片描述](https://img-blog.csdnimg.cn/20200915143416121.gif#pic_center)


### 字体设置

	设置编辑框字体和字体大小。

![在这里插入图片描述](https://img-blog.csdnimg.cn/20200915143500687.gif#pic_center)


### 颜色设置

	设置编辑框字体颜色。

![在这里插入图片描述](https://img-blog.csdnimg.cn/20200915143516635.gif#pic_center)


### 背景设置

	设置编辑框背景图片。

![在这里插入图片描述](https://img-blog.csdnimg.cn/20200915143557420.gif#pic_center)



Github下载地址：https://github.com/ys-lucky/QtCompiler.git

