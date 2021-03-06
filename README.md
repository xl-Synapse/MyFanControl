# MyFanControl
## 贴吧大神作品、好用度爆表、
原贴：https://tieba.baidu.com/p/5971634018?red_tag=1969604114
感谢hqnklwsy

## 使用说明
1. 安装NTPortDrvSetup,压缩包中有。
2. 管理员权限启动MyFanControl，非管理员权限启动无法设置开机自启。

## 原贴说明
1. 输入数值后要点保存才能生效。
2. 程序退出时会还原所有更改，包括还原风扇控制策略到原厂默认、解除GPU限频。
3. 风扇转速RPM可能不准确，因为dll获得的转速与实际转速是负相关的，所以我根据CC显示的转速自己拟合出来公式，我不能保证在别的电脑上显示的转速准确。
4. 过渡温度，为了避免风扇转速在2档之间频繁切换，温度下降时会延迟降低转速，比如在默认设置下，过渡温度3度，从75度温度上升到80度时转速会提高到70%，而温度下降时，需要温度低于80-3，也就是低于77度时转速才会降到55%。
5. 线性控制，平滑温度转速曲线。打开后，比如温度从60度上升到65时，每上升1度，转速会提高1%。
6. 强制冷却，强制风扇转速为95%，直到CPU和GPU温度都低于设定温度，我一般用于打完游戏关机前，把温度降下来再关机。
7. GPU限频，这只是限制GPU最大工作频率，在没有负载时，GPU频率仍然会降到更低。如果输入0则默认不限频，与不点勾的效果一致。此功能可能会与蓝天的CC冲突，因为CC的后台进程开机自启动时检测到限频后会调用同样的dll来取消限频。
