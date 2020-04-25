# javap-v
    github下载有时速度有点慢, 程序已经上传到百度云, 下载链接: https://pan.baidu.com/s/1BKF753IsLfU200VWdVGDkQ (提取码：kerl)  
    
    ![DEMO](https://raw.githubusercontent.com/juanyan711/javap-v/master/Resources/images/demo.png)

    javap-v的第一目标是完全替换javap这个系统工具, 提供比javap更加友好的输出展示, 是javap的GUI版本, 脱离jdk环境独立运行. 在优化展  
    示的前提下, javap-v完全等效于 javap -v 参数下的输出. 

    目前实现的主要功能有:
    1. 完全实现了javap -v下的结果输出.
    2. 通过一个树形控件来对javap命令的结果进行分块展示.
    3. 增加对class文件的字节显示。
    4. 点击树形结构中的分块时, 会在字节区域同步滚动且加亮显示对应的字节部分。
    
    编写javap-v的初衷是为了帮助java学习者更方便的去解读class文件.目前只支持jdk8及以下版本的class文件, 后续版本会支持到最新的jdk版本  
    和增加其他功能, 同时, 会增加常量池中的常量项目, CODE中的字节码对应字节的滚动显示. 反馈bug时, 请提交对应的java文件和jdk版本.

    若有侵权的地方, 请联系修改.

    