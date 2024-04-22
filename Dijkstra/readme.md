## step1：
设置一个优先级队列，用来存放待遍历的节点，命名为for_nodes
设置一个map，用来临时存放所有节点当前的最短距离，命名为optimal_nodes
临时迭代队列temp_for_nodes

## step2：
* 1、将起点0纳入 for_nodes
* 2、用node将for_nodes中的元素遍历，判断node的最短距离+到达邻居们的距离是否小于邻居们保存的历史最短距离，小于则更新邻居们的历史最短距离和上一个节点。如果邻居们没有被迭代过（标记过），加入到零时迭代向量temp_for_nodes。标记node为true；
* 3、for_nodes = temp_for_nodes；temp_for_nodes清除
* 4、转到2
* 5、等待for_nodes为空结束
* 6、返回optimal_nodes