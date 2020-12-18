# TabularQLearning_C
### Assignment for ENGG1110  

In this project, I utilise the **Reinforcement Learning** technique of **Tabular Q-Learning**, in order to train intelligent, competent Artificial Intelligence Tic Tac Toe playing agents. The AI agents are trained in an **adversarial** manner, starting from a random seed, against each other and against a random algorithm (to improve model robustness and generalisation).  

Tabular Q-Learning is a **model-free, dynamic-programming** based Reinforcement Learning algorithm. Reinforcement Learning (RL) is a branch of Machine Learning, which powers innovations such as AlphaGo. RL agents learn from trial and error, where rewards and penalties are assigned for each decision made for each state. The objective of an RL agent is to maximise its reward while minimising its penalty, which, for a well-designed objective function, should incrementally improve the performance of the AI. The Q-Learning algorithm is based on the Q-value iteration, where with each iteration, the agent learns the projected reward q_i given a state S_i. In Tabular Q-Learning, we try to find the optimal Q-values, via the following update policy:  
![](./Capture.PNG)  

Tabular Q-Learning is grounded on the principle of Dynamic Programming, which is an optimisation method similar to but more efficient than recursion (often linear time complexity). Evoking program flow models learned in ENGG1110, the tabulation method implemented in this project is a Bottom-Up approach. Q-value calculation is optimised starting from individual state-action pairs, accumulating increasingly sophisticated and optimised answers to the top, overall Q-table structure.
