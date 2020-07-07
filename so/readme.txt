$ gcc main.cpp -L. -lwt-calibration -o test

export LD_LIBRARY_PATH=$(pwd)


	for (i=0; i<SpectRslt.spectSize; i++)
	{
		printf("SpectRslt.pBuf[%d]=%lf\n",i,SpectRslt.pBuf[i]);
		power_sum+= SpectRslt.pBuf[i];
	}
	printf("strlen(file_buf)=%d\n",strlen(file_buf));
	//write_file("pinpu.csv", file_buf, strlen(file_buf));

	printf("SpectRslt.pBuf[600]=%lf,power_sum=%lf,avg=%lf\n",SpectRslt.pBuf[600],power_sum,power_sum/1200);