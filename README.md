# javap-v
    github上传下载速度慢, 最新的程序上传到了百度云, 下载链接:
	https://pan.baidu.com/s/1BKF753IsLfU200VWdVGDkQ (提取码：kerl)  

![DEMO](https://s1.ax1x.com/2020/04/25/J6PHot.png)

    要是DEMO图片看不到，可以直接访问: https://s1.ax1x.com/2020/04/25/J6PHot.png 查看. 或者在hosts中添加github的域名映射.
    
    javap-v的第一目标是完全替换javap这个系统工具, 提供比javap更加友好的输出展示和可操作性, 是javap的GUI版本. 其脱离jdk环境独  
    立运行. 在优化展示的前提下, javap-v完全等效于 javap -v 参数下的输出. 

    目前实现的主要功能有:
    1. 完全实现了javap -v下的结果输出.
    2. 通过一个树形控件来对javap命令的结果进行分块展示.
    3. 增加class文件的字节显示。
    4. 点击树形结构中的分块时, 会在字节区域同步滚动且加亮显示分块对应的字节部分。
    5. 采用Qt库来实现, 理论上可以做到跨平台.
    
    编写javap-v的初衷是为了帮助java学习者更方便的去解读class文件, 目前只支持jdk8及以下版本的class文件. 后续版本会增加如下功能:  
    1. 支持到最新的jdk版本.
    2. 增加常量池中的常量项目在字节区域的滚动显示.  
    3. 增加CODE中的字节码在字节区域的滚动显示.
    4. 国际化功能.
    5. 提供Linux, Mac下的可运行程序包.
    6. 待确定.
    
    反馈bug时, 请提交对应的java文件和jdk版本. 若有侵权的地方, 请联系修改.

    
