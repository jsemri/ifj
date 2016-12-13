%[1]
[s, Fs] = audioread('xsemri00.wav');
%Fs = 16 000 Length = 16 000 Length = 1 s.
%[2]
N = length(s);
f = Fs/N.*(0:N/2-1);
Y = fft(s,N);
Y = abs(Y(1:N/2))./(N/2);
plot(f,Y);
%[3]
find(Y==max(Y))-1)
%[4]
b = [0.2324 -0.4112 0.2324]
a = [1 0.2289 0.4662]
zplane(b,a);
%[5]
H = freqz(b,a,N/2);
plot(f,abs(H));
%[6]
out = filter(b,a,s);
Y = fft(out,N);
Y = abs(Y(1:N/2))./(N/2);
plot(f,Y);
%[7]
find(Y==max(Y))-1)
%[8]
% TODO

%[9]
Rv = xcorr(s, 'biased');
k = -50:50;
plot(k, Rv(N-1-50:N-1+50));

%[10]
Rv(10 + N)

%[11]
%[12]
%[13]
x = linspace(min(s), max(s), 100);
[h,p,r] = hist2opt(s(1:N-10), s(11:N), x);
imagesc(x,x,p);axis xy; colorbar;
disp(r);

