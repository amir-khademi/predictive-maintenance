# CRM_Activity_Prediction
A Simple approach for a CRM fault prediction system.
## How to use?
Just add your DataSet path in dataset_loader.py and enjoy!
You can modify the network by units parameter in DenseNN class.

## Known Issues:
if you change the units parameter for creating a customized network, loading the model will throw an exception. Try relocating the model checkpoint file. To solve thi issue, model name should use units parameter as well.
