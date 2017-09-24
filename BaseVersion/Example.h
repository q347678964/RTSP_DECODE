#ifndef EXAMPLE_H
#define EXAMPLE_H


class Example {

public:
	Example();
	~Example();

	void Printf(CString Context);
	void Example::ProcessCtrl(int Num);

	void FileOperation(void);

	void TimerTest(void);

	void ThreadTest(void);

	void Example::ProcessTest(void);

	void Example::GetHostAddress(CString &strIPAddr);
	void Example::NetTest(void);

	void Example::WinCMDTest(void);
};
#endif